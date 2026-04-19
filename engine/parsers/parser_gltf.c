gltf_data gltf_parse_file(const c8* file_path)
{
    platform_hnd file_handle     = platform_file_open(file_path, PLATFORM_FILE_FLAGS_read | PLATFORM_FILE_FLAGS_share_r);
    platform_file_info file_info = platform_file_info_get(file_handle);

    gltf_parser parser;

    cpu_arena_params params = { file_info.size * 2, file_info.size * 2, 0 };

    parser.arena       = cpu_arena_init(&params);
    parser.source.data = MEMORY_PUSH(parser.arena, u8, file_info.size);
    parser.source.size = platform_file_read(file_handle, parser.source.data, FILE_READ_ALL);
    parser.position    = 0;

    u32 magic   = *((u32*)(parser.source.data));
    u32 version = *((u32*)(parser.source.data + 4));
    u32 size    = *((u32*)(parser.source.data + 8));

    (void)version;

    if (magic != GLTF_MAGIC)
    {
        return (gltf_data){0};
    }

    parser.position = 12;

    gltf_json_data json_data = {0};
    gltf_data result         = {0};

    while (parser.position < size)
    {
        u32 chunk_length = *((u32*)(parser.source.data + parser.position));
        u32 chunk_type   = *((u32*)(parser.source.data + parser.position + 4));

        parser.position += 8;

        if (chunk_type == GLTF_CHUNK_JSON)
        {
            json_data = gltf_parse_chunk_json(&parser, chunk_length);
        }
        else if (chunk_type == GLTF_CHUNK_BIN)
        {
            result = gltf_parse_chunk_binary(&parser, chunk_length, &json_data);
        }
        else
        {
            EMBER_ASSERT(EMBER_FALSE);
        }

        parser.position += chunk_length;
    }

    result.arena = parser.arena;

    return result;
}

gltf_json_data gltf_parse_chunk_json(gltf_parser* parser, u32 chunk_length)
{
    (void)chunk_length;

    json_entry* json    = json_parse(parser->source.data + parser->position, chunk_length, parser->arena);
    json_entry* current = json->child;

    gltf_json_data result = {0};

    while (current != NULL)
    {
        buffer label_nodes  = buffer_from_cstr("nodes");
        buffer label_meshes = buffer_from_cstr("meshes");
        buffer label_access = buffer_from_cstr("accessors");
        buffer label_views  = buffer_from_cstr("bufferViews");
        buffer label_buffer = buffer_from_cstr("buffers");
        buffer label_tex    = buffer_from_cstr("textures");
        buffer label_img    = buffer_from_cstr("images");
        buffer label_sample = buffer_from_cstr("samplers");
        buffer label_mat    = buffer_from_cstr("materials");

        if (buffer_is_equal(&label_nodes, &current->label))
        {
            i32 node_count    = json_num_of_children(current);
            result.nodes      = MEMORY_PUSH(parser->arena, gltf_node, node_count);
            result.node_count = node_count;

            json_entry* node = current->child;
            for (i32 i = 0; i < node_count && node != NULL; i++)
            {
                i32  mesh;
                mat4 matrix;
                vec3 translation;
                quat rotation;
                vec3 scale;

                // TODO(KB): Fill missing info, if necessary

                b32 mesh_is_valid   = json_child_value(parser->arena, node, JSON_VALUE_TYPE_i32, &mesh, "mesh");
                b32 matrix_is_valid = json_child_value(parser->arena, node, JSON_VALUE_TYPE_arr_f32, &matrix, "matrix");
                b32 name_is_valid   = json_child_value(parser->arena, node, JSON_VALUE_TYPE_str, result.nodes[i].name, "name");

                (void)name_is_valid;

                b32 children_is_valid = EMBER_FALSE;
                b32 trs_is_valid      = EMBER_TRUE;

                i32 child_count           = 0;
                buffer children_label     = buffer_from_cstr("children");
                json_entry* children_node = json_find_child(node, &children_label);

                i32* children = NULL;
                if (children_node != NULL)
                {
                    child_count       = json_num_of_children(children_node);
                    children          = MEMORY_PUSH(parser->arena, i32, child_count);
                    children_is_valid = json_child_value(parser->arena, node, JSON_VALUE_TYPE_arr_i32, children, "children");

                    EMBER_ASSERT(children_is_valid);

                    for (i32 child = 0; child < child_count; child++)
                    {
                        i32 child_id = children[child];

                        result.nodes[child_id].parent     = i;
                        result.nodes[child_id].has_parent = EMBER_TRUE;
                    }
                }

                if (!matrix_is_valid)
                {
                    translation = VEC3_ZERO;
                    rotation    = QUAT_IDENTITY;
                    scale       = VEC3_ONE;

                    b32 pos_is_valid = json_child_value(parser->arena, node, JSON_VALUE_TYPE_arr_f32, &translation, "translation");
                    b32 rot_is_valid = json_child_value(parser->arena, node, JSON_VALUE_TYPE_arr_f32, &rotation, "rotation");
                    b32 scl_is_valid = json_child_value(parser->arena, node, JSON_VALUE_TYPE_arr_f32, &scale, "scale");

                    trs_is_valid = pos_is_valid || rot_is_valid || scl_is_valid;
                }

                result.nodes[i].matrix      = matrix_is_valid ? matrix : (trs_is_valid ? mat4_model(&translation, &rotation, &scale) : MAT4_IDENTITY);
                result.nodes[i].children    = children_is_valid ? children : NULL;
                result.nodes[i].child_count = child_count;
                result.nodes[i].mesh        = mesh_is_valid ? mesh : -1;
                result.nodes[i].parent      = result.nodes[i].has_parent ? result.nodes[i].parent : -1;

                node = node->next;
            }

        }
        else if (buffer_is_equal(&label_meshes, &current->label))
        {
            i32 mesh_count    = json_num_of_children(current);
            result.meshes     = MEMORY_PUSH(parser->arena, gltf_mesh, mesh_count);
            result.mesh_count = mesh_count;

            json_entry* mesh = current->child;
            for (i32 i = 0; i < mesh_count && mesh != NULL; i++)
            {
                i32 position;
                i32 normal;
                i32 tangent;
                i32 texcoord;
                i32 color;
                i32 joints;
                i32 weights;
                i32 indices;
                i32 material;
                i32 mode;

                // TODO(KB): Fill missing info, if necessary

                buffer primitives_buffer = buffer_from_cstr("primitives");
                buffer attributes_buffer = buffer_from_cstr("attributes");

                json_entry* current_prim = json_find_child(mesh, &primitives_buffer);
                i32 primitive_count      = json_num_of_children(current_prim);

                result.meshes[i].primitives      = MEMORY_PUSH(parser->arena, gltf_primitive, primitive_count);
                result.meshes[i].primitive_count = primitive_count;

                current_prim = current_prim->child;

                for (i32 j = 0; j < primitive_count; j++)
                {
                    EMBER_ASSERT(current_prim != NULL);

                    json_entry* attributes = json_find_child(current_prim, &attributes_buffer);

                    b32 position_is_valid = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &position, "POSITION");
                    b32 normal_is_valid   = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &normal, "NORMAL");
                    b32 tangent_is_valid  = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &tangent, "TANGENT");
                    b32 texcoord_is_valid = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &texcoord, "TEXCOORD_0");
                    b32 color_is_valid    = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &color, "COLOR_0");
                    b32 joints_is_valid   = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &joints, "JOINTS_0");
                    b32 weights_is_valid  = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &weights, "WEIGHTS_0");
                    b32 indices_is_valid  = json_child_value(parser->arena, current_prim, JSON_VALUE_TYPE_i32, &indices, "indices");
                    b32 material_is_valid = json_child_value(parser->arena, current_prim, JSON_VALUE_TYPE_i32, &material, "material");
                    b32 mode_is_valid     = json_child_value(parser->arena, current_prim, JSON_VALUE_TYPE_i32, &mode, "mode");

                    result.meshes[i].primitives[j].position  = position_is_valid ? position : -1;
                    result.meshes[i].primitives[j].normal    = normal_is_valid ? normal : -1;
                    result.meshes[i].primitives[j].tangent   = tangent_is_valid ? tangent : -1;
                    result.meshes[i].primitives[j].texcoord  = texcoord_is_valid ? texcoord : -1;
                    result.meshes[i].primitives[j].color     = color_is_valid ? color : -1;
                    result.meshes[i].primitives[j].joints    = joints_is_valid ? joints : -1;
                    result.meshes[i].primitives[j].weights   = weights_is_valid ? weights : -1;
                    result.meshes[i].primitives[j].indices   = indices_is_valid ? indices : -1;
                    result.meshes[i].primitives[j].material  = material_is_valid ? material : -1;
                    result.meshes[i].primitives[j].draw_mode = mode_is_valid ? mode : -1;

                    result.primitive_count += 1;

                    current_prim = current_prim->next;
                }

                mesh = mesh->next;
            }
        }
        else if (buffer_is_equal(&label_access, &current->label))
        {
            i32 accessor_count    = json_num_of_children(current);
            result.accessors      = MEMORY_PUSH(parser->arena, gltf_accessor, accessor_count);
            result.accessor_count = accessor_count;

            json_entry* accessor = current->child;
            for (i32 i = 0; i < accessor_count && accessor != NULL; i++)
            {
                i32 buffer_view_id;
                i32 byte_offset;
                i32 component_type;
                i32 count;
                c8 type[16];

                // TODO(KB): Fill missing info, if necessary

                b32 buffer_view_id_is_valid = json_child_value(parser->arena, accessor, JSON_VALUE_TYPE_i32, &buffer_view_id, "bufferView");
                b32 byte_offset_is_valid    = json_child_value(parser->arena, accessor, JSON_VALUE_TYPE_i32, &byte_offset, "byteOffset");
                b32 component_type_is_valid = json_child_value(parser->arena, accessor, JSON_VALUE_TYPE_i32, &component_type, "componentType");
                b32 count_is_valid          = json_child_value(parser->arena, accessor, JSON_VALUE_TYPE_i32, &count, "count");
                b32 type_is_valid           = json_child_value(parser->arena, accessor, JSON_VALUE_TYPE_str, &type, "type");

                result.accessors[i].buffer_view_id = buffer_view_id_is_valid ? buffer_view_id : -1;
                result.accessors[i].byte_offset    = byte_offset_is_valid ? byte_offset : 0;
                result.accessors[i].component_type = component_type_is_valid ? component_type : -1;
                result.accessors[i].count          = count_is_valid ? count : 0;

                result.accessors[i].type = -1;
                if (type_is_valid)
                {
                    if (type[0] == 'S')
                    {
                        result.accessors[i].type = GLTF_CUSTOM_TYPE_SCALAR;
                    }
                    else if (type[0] == 'V')
                    {
                        result.accessors[i].type = GLTF_CUSTOM_TYPE_VEC2 + (type[3] - '2');
                    }
                    else if (type[0] == 'M')
                    {
                        result.accessors[i].type = GLTF_CUSTOM_TYPE_MAT2 + (type[3] - '2');
                    }
                }

                accessor = accessor->next;
            }
        }
        else if (buffer_is_equal(&label_views, &current->label))
        {
            i32 view_count           = json_num_of_children(current);
            result.buffer_views      = MEMORY_PUSH(parser->arena, gltf_buffer_view, view_count);
            result.buffer_view_count = view_count;

            json_entry* view = current->child;
            for (i32 i = 0; i < view_count && view != NULL; i++)
            {
                i32 buf_id;
                i32 offset;
                i32 length;
                i32 stride;
                i32 target;

                b32 buf_id_is_valid = json_child_value(parser->arena, view, JSON_VALUE_TYPE_i32, &buf_id, "buffer");
                b32 offset_is_valid = json_child_value(parser->arena, view, JSON_VALUE_TYPE_i32, &offset, "byteOffset");
                b32 length_is_valid = json_child_value(parser->arena, view, JSON_VALUE_TYPE_i32, &length, "byteLength");
                b32 stride_is_valid = json_child_value(parser->arena, view, JSON_VALUE_TYPE_i32, &stride, "byteStride");
                b32 target_is_valid = json_child_value(parser->arena, view, JSON_VALUE_TYPE_i32, &target, "target");

                result.buffer_views[i].buffer_id   = buf_id_is_valid ? buf_id : -1;
                result.buffer_views[i].byte_offset = offset_is_valid ? offset : 0;
                result.buffer_views[i].byte_length = length_is_valid ? length : 0;
                result.buffer_views[i].byte_stride = stride_is_valid ? stride : 0;
                result.buffer_views[i].target      = target_is_valid ? target : -1;

                view = view->next;
            }
        }
        else if (buffer_is_equal(&label_buffer, &current->label))
        {
            i32 buffer_count    = json_num_of_children(current);
            result.buffers      = MEMORY_PUSH(parser->arena, gltf_buffer, buffer_count);
            result.buffer_count = buffer_count;

            json_entry* buffer = current->child;
            for (i32 i = 0; i < buffer_count && buffer != NULL; i++)
            {
                i32 length;

                b32 length_is_valid = json_child_value(parser->arena, buffer, JSON_VALUE_TYPE_i32, &length, "byteLength");

                result.buffers[i].byte_length = length_is_valid ? length : -1;
            }

            buffer = buffer->next;
        }
        else if (buffer_is_equal(&label_tex, &current->label))
        {
            i32 texture_count    = json_num_of_children(current);
            result.textures      = MEMORY_PUSH(parser->arena, gltf_texture, texture_count);
            result.texture_count = texture_count;

            json_entry* tex = current->child;
            for (i32 i = 0; i < texture_count && tex != NULL; i++)
            {
                i32 sampler;
                i32 source;

                b32 sampler_is_valid = json_child_value(parser->arena, tex, JSON_VALUE_TYPE_i32, &sampler, "sampler");
                b32 source_is_valid  = json_child_value(parser->arena, tex, JSON_VALUE_TYPE_i32, &source, "source");

                result.textures[i].sampler = sampler_is_valid ? sampler : -1;
                result.textures[i].image   = source_is_valid ? source : -1;
            }
        }
        else if (buffer_is_equal(&label_img, &current->label))
        {
            i32 image_count    = json_num_of_children(current);
            result.images      = MEMORY_PUSH(parser->arena, gltf_image, image_count);
            result.image_count = image_count;

            json_entry* image = current->child;
            for (i32 i = 0; i < image_count && image != NULL; i++)
            {
                i32 buf_view;
                c8 mime_type[16] = {0};

                b32 buf_view_is_valid  = json_child_value(parser->arena, image, JSON_VALUE_TYPE_i32, &buf_view, "bufferView");
                b32 mime_type_is_valid = json_child_value(parser->arena, image, JSON_VALUE_TYPE_str, &mime_type, "mimeType");

                // NOTE(KB): We do not support external images for now
                EMBER_ASSERT(buf_view_is_valid);

                result.images[i].buffer_view_id = buf_view_is_valid ? buf_view : -1;

                EMBER_ASSERT(mime_type_is_valid);

                buffer png      = buffer_from_cstr("image/png");
                buffer jpeg     = buffer_from_cstr("image/jpeg");
                buffer mime_buf = buffer_from_cstr(mime_type);

                if (buffer_is_equal(&mime_buf, &png))
                {
                    result.images[i].mime_type = GLTF_MIME_TYPE_PNG;
                }
                else if (buffer_is_equal(&mime_buf, &jpeg))
                {
                    result.images[i].mime_type = GLTF_MIME_TYPE_JPEG;
                }
                else
                {
                    EMBER_ASSERT(EMBER_FALSE);
                }
            }
        }
        else if (buffer_is_equal(&label_sample, &current->label))
        {
            i32 sampler_count    = json_num_of_children(current);
            result.samplers      = MEMORY_PUSH(parser->arena, gltf_sampler, sampler_count);
            result.sampler_count = sampler_count;

            json_entry* sampler = current->child;
            for (i32 i = 0; i < sampler_count && sampler != NULL; i++)
            {
                i32 min_filter;
                i32 mag_filter;
                i32 wrap_u;
                i32 wrap_v;

                b32 min_filter_is_valid = json_child_value(parser->arena, sampler, JSON_VALUE_TYPE_i32, &min_filter, "minFilter");
                b32 mag_filter_is_valid = json_child_value(parser->arena, sampler, JSON_VALUE_TYPE_i32, &mag_filter, "magFilter");
                b32 wrap_u_is_valid     = json_child_value(parser->arena, sampler, JSON_VALUE_TYPE_i32, &wrap_u, "wrapS");
                b32 wrap_v_is_valid     = json_child_value(parser->arena, sampler, JSON_VALUE_TYPE_i32, &wrap_v, "wrapV");

                result.samplers[i].min_filter = min_filter_is_valid ? min_filter : -1;
                result.samplers[i].mag_filter = mag_filter_is_valid ? mag_filter : -1;
                result.samplers[i].wrap_u     = wrap_u_is_valid ? wrap_u : -1;
                result.samplers[i].wrap_v     = wrap_v_is_valid ? wrap_v : -1;
            }
        }
        else if (buffer_is_equal(&label_mat, &current->label))
        {
            i32 material_count    = json_num_of_children(current);
            result.materials      = MEMORY_PUSH(parser->arena, gltf_material, material_count);
            result.material_count = material_count;

            buffer buffer_pbr     = buffer_from_cstr("pbrMetallicRoughness");
            buffer buffer_tex_clr = buffer_from_cstr("baseColorTexture");
            buffer buffer_tex_mr  = buffer_from_cstr("metallicRoughnessTexture");
            buffer buffer_tex_nm  = buffer_from_cstr("normalTexture");
            buffer buffer_tex_ao  = buffer_from_cstr("occlusionTexture");
            buffer buffer_tex_em  = buffer_from_cstr("emissiveTexture");

            json_entry* mat = current->child;
            for (i32 i = 0; i < material_count && mat != NULL; i++)
            {
                f32 pbr_clr[4]     = {1.0f, 1.0f, 1.0f, 1.0f};
                i32 pbr_tex_clr_id = -1;
                i32 pbr_tex_clr_uv = 0;
                f32 pbr_metal      = 1.0f;
                f32 pbr_rough      = 1.0f;
                i32 pbr_tex_mr_id  = -1;
                i32 pbr_tex_mr_uv  = 0;
                i32 tex_nm_id      = -1;
                i32 tex_nm_uv      = 0;
                f32 tex_nm_scale   = 1.0f;
                i32 tex_ao_id      = -1;
                i32 tex_ao_uv      = 0;
                f32 tex_ao_str     = 0;
                i32 tex_em_id      = -1;
                i32 tex_em_uv      = 0;
                f32 emissive[3]    = {0.0f, 0.0f, 0.0f};
                c8 alpha_mode[8]   = {0};
                f32 alpha_cutoff   = 0.5f;
                b32 double_sided   = EMBER_FALSE;

                json_entry* pbr = json_find_child(mat, &buffer_pbr);

                b32 pbr_clr_is_valid        = EMBER_FALSE;
                b32 pbr_tex_clr_id_is_valid = EMBER_FALSE;
                b32 pbr_tex_clr_uv_is_valid = EMBER_FALSE;
                b32 pbr_metal_is_valid      = EMBER_FALSE;
                b32 pbr_rough_is_valid      = EMBER_FALSE;
                b32 pbr_tex_mr_id_is_valid  = EMBER_FALSE;
                b32 pbr_tex_mr_uv_is_valid  = EMBER_FALSE;

                if (pbr != NULL)
                {
                    json_entry* tex_clr = json_find_child(pbr, &buffer_tex_clr);
                    json_entry* tex_mr  = json_find_child(pbr, &buffer_tex_mr);

                    pbr_clr_is_valid   = json_child_value(parser->arena, pbr, JSON_VALUE_TYPE_arr_f32, &pbr_clr, "baseColorFactor");
                    pbr_metal_is_valid = json_child_value(parser->arena, pbr, JSON_VALUE_TYPE_f32, &pbr_metal, "metallicFactor");
                    pbr_rough_is_valid = json_child_value(parser->arena, pbr, JSON_VALUE_TYPE_f32, &pbr_rough, "roughnessFactor");

                    if (tex_clr != NULL)
                    {
                        pbr_tex_clr_id_is_valid = json_child_value(parser->arena, tex_clr, JSON_VALUE_TYPE_i32, &pbr_tex_clr_id, "index");
                        pbr_tex_clr_uv_is_valid = json_child_value(parser->arena, tex_clr, JSON_VALUE_TYPE_i32, &pbr_tex_clr_uv, "texCoord");
                    }

                    if (tex_mr != NULL)
                    {
                        pbr_tex_mr_id_is_valid = json_child_value(parser->arena, tex_mr, JSON_VALUE_TYPE_i32, &pbr_tex_mr_id, "index");
                        pbr_tex_mr_uv_is_valid = json_child_value(parser->arena, tex_mr, JSON_VALUE_TYPE_i32, &pbr_tex_mr_uv, "texCoord");
                    }
                }

                json_entry* tex_nm = json_find_child(mat, &buffer_tex_nm);

                b32 tex_nm_id_is_valid    = EMBER_FALSE;
                b32 tex_nm_uv_is_valid    = EMBER_FALSE;
                b32 tex_nm_scale_is_valid = EMBER_FALSE;

                if (tex_nm != NULL)
                {
                    tex_nm_id_is_valid    = json_child_value(parser->arena, tex_nm, JSON_VALUE_TYPE_i32, &tex_nm_id, "index");
                    tex_nm_uv_is_valid    = json_child_value(parser->arena, tex_nm, JSON_VALUE_TYPE_i32, &tex_nm_uv, "texCoord");
                    tex_nm_scale_is_valid = json_child_value(parser->arena, tex_nm, JSON_VALUE_TYPE_f32, &tex_nm_scale, "scale");
                }

                json_entry* tex_ao = json_find_child(mat, &buffer_tex_ao);

                b32 tex_ao_id_is_valid  = EMBER_FALSE;
                b32 tex_ao_uv_is_valid  = EMBER_FALSE;
                b32 tex_ao_str_is_valid = EMBER_FALSE;

                if (tex_ao != NULL)
                {
                    tex_ao_id_is_valid  = json_child_value(parser->arena, tex_ao, JSON_VALUE_TYPE_i32, &tex_ao_id, "index");
                    tex_ao_uv_is_valid  = json_child_value(parser->arena, tex_ao, JSON_VALUE_TYPE_i32, &tex_ao_uv, "texCoord");
                    tex_ao_str_is_valid = json_child_value(parser->arena, tex_ao, JSON_VALUE_TYPE_f32, &tex_ao_str, "strength");
                }

                json_entry* tex_em = json_find_child(mat, &buffer_tex_em);

                b32 tex_em_id_is_valid = EMBER_FALSE;
                b32 tex_em_uv_is_valid = EMBER_FALSE;

                if (tex_em != NULL)
                {
                    tex_em_id_is_valid = json_child_value(parser->arena, tex_em, JSON_VALUE_TYPE_i32, &tex_em_id, "index");
                    tex_em_uv_is_valid = json_child_value(parser->arena, tex_em, JSON_VALUE_TYPE_i32, &tex_em_uv, "texCoord");
                }

                b32 emissive_is_valid     = json_child_value(parser->arena, mat, JSON_VALUE_TYPE_arr_f32, &emissive, "emissiveFactor");
                b32 alpha_mode_is_valid   = json_child_value(parser->arena, mat, JSON_VALUE_TYPE_i32, &alpha_mode, "alphaMode");
                b32 alpha_cutoff_is_valid = json_child_value(parser->arena, mat, JSON_VALUE_TYPE_f32, &alpha_cutoff, "alphaCutoff");
                b32 double_sided_is_valid = json_child_value(parser->arena, mat, JSON_VALUE_TYPE_b32, &double_sided, "doubleSided");

                result.materials[i].pbr_tex_clr_id = pbr_tex_clr_id_is_valid ? pbr_tex_clr_id : -1;
                result.materials[i].pbr_tex_clr_uv = pbr_tex_clr_uv_is_valid ? pbr_tex_clr_uv : 0;
                result.materials[i].pbr_metal      = pbr_metal_is_valid ? pbr_metal : 1.0f;
                result.materials[i].pbr_rough      = pbr_rough_is_valid ? pbr_rough : 1.0f;
                result.materials[i].pbr_tex_mr_id  = pbr_tex_mr_id_is_valid ? pbr_tex_mr_id : -1;
                result.materials[i].pbr_tex_mr_uv  = pbr_tex_mr_uv_is_valid ? pbr_tex_mr_uv : 0;
                result.materials[i].tex_nm_id      = tex_nm_id_is_valid ? tex_nm_id : -1;
                result.materials[i].tex_nm_uv      = tex_nm_uv_is_valid ? tex_nm_uv : 0;
                result.materials[i].tex_nm_scale   = tex_nm_scale_is_valid ? tex_nm_scale : 1.0f;
                result.materials[i].tex_ao_id      = tex_ao_id_is_valid ? tex_ao_id : -1;
                result.materials[i].tex_ao_uv      = tex_ao_uv_is_valid ? tex_ao_uv : 0;
                result.materials[i].tex_ao_str     = tex_ao_str_is_valid ? tex_ao_str : 1.0f;
                result.materials[i].tex_em_id      = tex_em_id_is_valid ? tex_em_id : -1;
                result.materials[i].tex_em_uv      = tex_em_uv_is_valid ? tex_em_uv : 0;
                result.materials[i].alpha_mode     = GLTF_ALPHA_MODE_OPAQUE;
                result.materials[i].alpha_cutoff   = alpha_cutoff_is_valid ? alpha_cutoff : 0.5f;
                result.materials[i].double_sided   = double_sided_is_valid ? double_sided : EMBER_FALSE;

                if (alpha_mode_is_valid)
                {
                    buffer buf_mask     = buffer_from_cstr("MASK");
                    buffer buf_blend    = buffer_from_cstr("BLEND");
                    buffer buf_mode_val = buffer_from_cstr(alpha_mode);

                    if (buffer_is_equal(&buf_mask, &buf_mode_val))
                    {
                        result.materials[i].alpha_mode = GLTF_ALPHA_MODE_MASK;
                    }
                    else if (buffer_is_equal(&buf_blend, &buf_mode_val))
                    {
                        result.materials[i].alpha_mode = GLTF_ALPHA_MODE_BLEND;
                    }
                    else
                    {
                        EMBER_ASSERT(EMBER_FALSE);
                    }
                }

                if (pbr_clr_is_valid)
                {
                    memcpy(result.materials[i].pbr_clr, pbr_clr, 4);
                }
                else
                {
                    result.materials[i].pbr_clr[0] = 1.0f;
                    result.materials[i].pbr_clr[1] = 1.0f;
                    result.materials[i].pbr_clr[2] = 1.0f;
                    result.materials[i].pbr_clr[3] = 1.0f;
                }

                if (emissive_is_valid)
                {
                    memcpy(result.materials[i].emissive, emissive, 3);
                }
                else
                {
                    result.materials[i].emissive[0] = 0.0f;
                    result.materials[i].emissive[1] = 0.0f;
                    result.materials[i].emissive[2] = 0.0f;
                }
            }
        }

        current = current->next;
    }

    return result;
}

gltf_data gltf_parse_chunk_binary(gltf_parser* parser, u32 chunk_length, gltf_json_data* json_data)
{
    (void)chunk_length;

    u8* data = parser->source.data + parser->position;

    gltf_data result = {0};

    EMBER_ASSERT(json_data != NULL);

    result.transforms       = MEMORY_PUSH(parser->arena, mat4, json_data->node_count);
    result.children         = MEMORY_PUSH(parser->arena, i32*, json_data->node_count);
    result.children_count   = MEMORY_PUSH(parser->arena, i32, json_data->node_count);
    result.parents          = MEMORY_PUSH(parser->arena, i32, json_data->node_count);
    result.mesh_ids         = MEMORY_PUSH(parser->arena, i32, json_data->node_count);
    result.mesh_offsets     = MEMORY_PUSH(parser->arena, i32, json_data->mesh_count);
    result.mesh_primitives  = MEMORY_PUSH(parser->arena, i32, json_data->mesh_count);
    result.meshes           = MEMORY_PUSH(parser->arena, mesh, json_data->primitive_count);
    result.textures         = MEMORY_PUSH(parser->arena, texture, json_data->texture_count);
    result.materials        = MEMORY_PUSH(parser->arena, material, json_data->material_count);
    result.node_count       = json_data->node_count;
    result.mesh_count       = json_data->mesh_count;
    result.primitive_count  = json_data->primitive_count;
    result.texture_count    = json_data->texture_count;
    result.material_count   = json_data->material_count;

    // NOTE(KB): Convert node data to output format
    for (i32 node_idx = 0; node_idx < json_data->node_count; node_idx++)
    {
        gltf_node* node = &json_data->nodes[node_idx];

        result.transforms[node_idx]     = node->matrix;
        result.children[node_idx]       = node->children;
        result.children_count[node_idx] = node->child_count;
        result.parents[node_idx]        = node->parent;
        result.mesh_ids[node_idx]       = node->mesh;
    }

    // NOTE(KB): Convert material data to output format
    for (i32 mat_idx = 0; mat_idx < json_data->material_count; mat_idx++)
    {
        gltf_material* mat = &json_data->materials[mat_idx];

        result.materials[mat_idx].color        = *((color4*)mat->pbr_clr);
        result.materials[mat_idx].color_em     = *((color3*)mat->emissive);
        result.materials[mat_idx].metal        = mat->pbr_metal;
        result.materials[mat_idx].rough        = mat->pbr_rough;
        result.materials[mat_idx].ao           = mat->tex_ao_str;
        result.materials[mat_idx].alpha_mode   = mat->alpha_mode;
        result.materials[mat_idx].alpha_cutoff = mat->alpha_cutoff;
        result.materials[mat_idx].normal_scale = mat->tex_nm_scale;
        result.materials[mat_idx].double_sided = mat->double_sided;
        result.materials[mat_idx].tex_id_al    = mat->pbr_tex_clr_id;
        result.materials[mat_idx].tex_id_mr    = mat->pbr_tex_mr_id;
        result.materials[mat_idx].tex_id_nm    = mat->tex_nm_id;
        result.materials[mat_idx].tex_id_ao    = mat->tex_ao_id;
        result.materials[mat_idx].tex_id_em    = mat->tex_em_id;
        result.materials[mat_idx].tex_uv_al    = mat->pbr_tex_clr_uv;
        result.materials[mat_idx].tex_uv_mr    = mat->pbr_tex_mr_uv;
        result.materials[mat_idx].tex_uv_nm    = mat->tex_nm_uv;
        result.materials[mat_idx].tex_uv_ao    = mat->tex_ao_uv;
        result.materials[mat_idx].tex_uv_em    = mat->tex_em_uv;
    }

    // NOTE(KB): Convert image/texture data to output format
    for (i32 texture_idx = 0; texture_idx < json_data->texture_count; texture_idx++)
    {
        gltf_texture* tex     = &json_data->textures[texture_idx];
        gltf_sampler* sampler = &json_data->samplers[tex->sampler];
        gltf_image* image     = &json_data->images[tex->image];
        gltf_buffer_view view = json_data->buffer_views[image->buffer_view_id];

        result.textures[texture_idx].type = image->mime_type;
        result.textures[texture_idx].size = view.byte_length;

        if (sampler->min_filter > GLTF_MIN_FILTER_LIN)
        {
            result.textures[texture_idx].sampler.min_filter = (i8)(
                sampler->min_filter
                - GLTF_MIN_FILTER_NEAR_MIP_NEAR
                + SAMPLER_MIN_FILTER_NEAR_MIP_NEAR
            );
        }
        else
        {
            result.textures[texture_idx].sampler.min_filter = (i8)(sampler->min_filter - GLTF_MIN_FILTER_NEAR);
        }

        result.textures[texture_idx].sampler.mag_filter = (i8)(sampler->mag_filter - GLTF_MAG_FILTER_NEAR);

        switch(sampler->wrap_u)
        {
            case GLTF_WRAP_REPEAT:
            {
                result.textures[texture_idx].sampler.wrap_u = SAMPLER_WRAP_REPEAT;
                break;
            }
            case GLTF_WRAP_CLAMP_TO_EDGE:
            {
                result.textures[texture_idx].sampler.wrap_u = SAMPLER_WRAP_CLAMP_TO_EDGE;
                break;
            }
            case GLTF_WRAP_MIRRORED_REPEAT:
            {
                result.textures[texture_idx].sampler.wrap_u = SAMPLER_WRAP_MIRRORED_REPEAT;
                break;
            }
        }

        switch(sampler->wrap_v)
        {
            case GLTF_WRAP_REPEAT:
            {
                result.textures[texture_idx].sampler.wrap_v = SAMPLER_WRAP_REPEAT;
                break;
            }
            case GLTF_WRAP_CLAMP_TO_EDGE:
            {
                result.textures[texture_idx].sampler.wrap_v = SAMPLER_WRAP_CLAMP_TO_EDGE;
                break;
            }
            case GLTF_WRAP_MIRRORED_REPEAT:
            {
                result.textures[texture_idx].sampler.wrap_v = SAMPLER_WRAP_MIRRORED_REPEAT;
                break;
            }
        }

        result.textures[texture_idx].data = (data + view.byte_offset);
    }

    // NOTE(KB): This is needed because we consider every unique primitive entry a new mesh
    u32 mesh_offset = 0;

    // NOTE(KB): Convert mesh data to output format
    for (i32 mesh_idx = 0; mesh_idx < json_data->mesh_count; mesh_idx++)
    {
        result.mesh_offsets[mesh_idx]    = mesh_offset;
        result.mesh_primitives[mesh_idx] = json_data->meshes[mesh_idx].primitive_count;

        for (i32 prim_idx = 0; prim_idx < json_data->meshes[mesh_idx].primitive_count; prim_idx++)
        {
            i32 acs_pos = json_data->meshes[mesh_idx].primitives[prim_idx].position;
            i32 acs_nrm = json_data->meshes[mesh_idx].primitives[prim_idx].normal;
            i32 acs_uvs = json_data->meshes[mesh_idx].primitives[prim_idx].texcoord;
            i32 acs_ids = json_data->meshes[mesh_idx].primitives[prim_idx].indices;

            i32 data_offset_pos = json_data->buffer_views[json_data->accessors[acs_pos].buffer_view_id].byte_offset
                                + json_data->accessors[acs_pos].byte_offset;

            i32 data_offset_nrm = json_data->buffer_views[json_data->accessors[acs_nrm].buffer_view_id].byte_offset
                                + json_data->accessors[acs_nrm].byte_offset;

            i32 data_offset_uvs = json_data->buffer_views[json_data->accessors[acs_uvs].buffer_view_id].byte_offset
                                + json_data->accessors[acs_uvs].byte_offset;

            i32 data_offset_ids = json_data->buffer_views[json_data->accessors[acs_ids].buffer_view_id].byte_offset
                                  + json_data->accessors[acs_ids].byte_offset;

            EMBER_ASSERT(
                (json_data->accessors[acs_pos].count == json_data->accessors[acs_nrm].count) &&
                (json_data->accessors[acs_pos].count == json_data->accessors[acs_uvs].count)
            );

            result.meshes[mesh_offset].vertex_count = json_data->accessors[acs_pos].count;
            result.meshes[mesh_offset].index_count  = json_data->accessors[acs_ids].count;

            result.meshes[mesh_offset].vertices = MEMORY_PUSH(parser->arena, vertex, result.meshes[mesh_offset].vertex_count);
            result.meshes[mesh_offset].indices  = MEMORY_PUSH(parser->arena, i32,  result.meshes[mesh_offset].index_count);

            gltf_parse_components(
                (data + data_offset_ids),
                result.meshes[mesh_offset].indices,
                result.meshes[mesh_offset].index_count,
                0,
                sizeof(i32),
                json_data->accessors[acs_ids].component_type,
                json_data->accessors[acs_ids].type
            );

            gltf_parse_components(
                (data + data_offset_pos),
                result.meshes[mesh_offset].vertices,
                result.meshes[mesh_offset].vertex_count,
                offsetof(vertex, position),
                sizeof(vertex),
                json_data->accessors[acs_pos].component_type,
                json_data->accessors[acs_pos].type
            );

            gltf_parse_components(
                (data + data_offset_nrm),
                result.meshes[mesh_offset].vertices,
                result.meshes[mesh_offset].vertex_count,
                offsetof(vertex, normal),
                sizeof(vertex),
                json_data->accessors[acs_nrm].component_type,
                json_data->accessors[acs_nrm].type
            );

            gltf_parse_components(
                (data + data_offset_uvs),
                result.meshes[mesh_offset].vertices,
                result.meshes[mesh_offset].vertex_count,
                offsetof(vertex, uv),
                sizeof(vertex),
                json_data->accessors[acs_uvs].component_type,
                json_data->accessors[acs_uvs].type
            );

            // NOTE(): Manually write to the color value
            for (i32 i = 0; i < result.meshes[mesh_offset].vertex_count; i++)
            {
                result.meshes[mesh_offset].vertices[i].color = (vec3){1.0f, 1.0f, 1.0f};
            }

            mesh_offset += 1;
        }
    }

    return result;
}

void gltf_parse_components(void* source, void* dest, i32 count, i32 offset, i32 stride, i32 cmp_type, i32 data_type)
{
    offset /= 4;
    stride /= 4;

    u8 data_type_size = 1;
    switch (data_type)
    {
        case GLTF_CUSTOM_TYPE_SCALAR:
        {
            data_type_size = 1;
            break;
        }
        case GLTF_CUSTOM_TYPE_VEC2:
        {
            data_type_size = 2;
            break;
        }
        case GLTF_CUSTOM_TYPE_VEC3:
        {
            data_type_size = 3;
            break;
        }
        case GLTF_CUSTOM_TYPE_VEC4:
        {
            data_type_size = 4;
            break;
        }
        case GLTF_CUSTOM_TYPE_MAT2:
        {
            data_type_size = 4;
            break;
        }
        case GLTF_CUSTOM_TYPE_MAT3:
        {
            data_type_size = 9;
            break;
        }
        case GLTF_CUSTOM_TYPE_MAT4:
        {
            data_type_size = 16;
            break;
        }
    }
    
    switch (cmp_type)
    {
        case GLTF_COMPONENT_TYPE_BYTE:
        case GLTF_COMPONENT_TYPE_UBYTE:
        {
            u8* src_data  = (u8 *)source;
            u32* dst_data = (u32 *)dest;

            for (i32 i = 0; i < count; i++)
            {
                for (u8 j = 0; j < data_type_size; j++)
                {
                    *(dst_data + i * stride + offset + j) = *(src_data + i * data_type_size + j);
                }
            }

            break;
        }
        case GLTF_COMPONENT_TYPE_SHORT:
        case GLTF_COMPONENT_TYPE_USHORT:
        {
            u16* src_data = (u16 *)source;
            u32* dst_data = (u32 *)dest;

            for (i32 i = 0; i < count; i++)
            {
                for (u8 j = 0; j < data_type_size; j++)
                {
                    *(dst_data + i * stride + offset + j) = *(src_data + i * data_type_size + j);
                }
            }

            break;
        }
        case GLTF_COMPONENT_TYPE_UINT:
        {
            u32* src_data = (u32 *)source;
            u32* dst_data = (u32 *)dest;

            for (i32 i = 0; i < count; i++)
            {
                for (u8 j = 0; j < data_type_size; j++)
                {
                    *(dst_data + i * stride + offset + j) = *(src_data + i * data_type_size + j);
                }
            }

            break;
        }
        case GLTF_COMPONENT_TYPE_FLOAT:
        {
            f32* src_data = (f32 *)source;
            f32* dst_data = (f32 *)dest;

            for (i32 i = 0; i < count; i++)
            {
                for (u8 j = 0; j < data_type_size; j++)
                {
                    *(dst_data + i * stride + offset + j) = *(src_data + i * data_type_size + j);
                }
            }

            break;
        }
        default:
        {
            EMBER_ASSERT(EMBER_FALSE);
        }
    }
}

void gltf_free(gltf_data* gltf)
{
    cpu_arena_release(gltf->arena);
}
