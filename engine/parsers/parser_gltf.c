gltf_data gltf_parse_file(const c8* file_path, cpu_arena* arena)
{
    platform_hnd file_handle     = platform_file_open(file_path, PLATFORM_FILE_FLAGS_read | PLATFORM_FILE_FLAGS_share_r);
    platform_file_info file_info = platform_file_info_get(file_handle);

    EMBER_ASSERT(file_info.size <= cpu_arena_avail(arena));

    gltf_parser parser;

    parser.arena       = arena;
    parser.source.data = MEMORY_PUSH(arena, u8, file_info.size);
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
        buffer nodes_label  = buffer_from_cstr("nodes");
        buffer meshes_label = buffer_from_cstr("meshes");
        buffer access_label = buffer_from_cstr("accessors");
        buffer views_label  = buffer_from_cstr("bufferViews");
        buffer buffer_label = buffer_from_cstr("buffers");

        if (buffer_is_equal(&nodes_label, &current->label))
        {
            u32 node_count    = json_num_of_children(current);
            result.nodes      = MEMORY_PUSH(parser->arena, gltf_node, node_count);
            result.node_count = node_count;

            json_entry* node = current->child;
            for (u32 i = 0; i < node_count && node != NULL; i++)
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

                u32 child_count           = 0;
                buffer children_label     = buffer_from_cstr("children");
                json_entry* children_node = json_find_child(node, &children_label);

                i32* children = NULL;
                if (children_node != NULL)
                {
                    child_count       = json_num_of_children(children_node);
                    children          = MEMORY_PUSH(parser->arena, i32, child_count);
                    children_is_valid = json_child_value(parser->arena, node, JSON_VALUE_TYPE_arr_i32, children, "children");

                    EMBER_ASSERT(children_is_valid);

                    for (u32 child = 0; child < child_count; child++)
                    {
                        u32 child_id = children[child];

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
        else if (buffer_is_equal(&meshes_label, &current->label))
        {
            u32 mesh_count    = json_num_of_children(current);
            result.meshes     = MEMORY_PUSH(parser->arena, gltf_mesh, mesh_count);
            result.mesh_count = mesh_count;

            json_entry* mesh = current->child;
            for (u32 i = 0; i < mesh_count && mesh != NULL; i++)
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

                json_entry* primitives = json_find_child(mesh, &primitives_buffer);
                primitives             = primitives->child;

                json_entry* attributes = json_find_child(primitives, &attributes_buffer);

                b32 position_is_valid = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &position, "POSITION");
                b32 normal_is_valid   = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &normal, "NORMAL");
                b32 tangent_is_valid  = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &tangent, "TANGENT");
                b32 texcoord_is_valid = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &texcoord, "TEXCOORD_0");
                b32 color_is_valid    = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &color, "COLOR_0");
                b32 joints_is_valid   = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &joints, "JOINTS_0");
                b32 weights_is_valid  = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &weights, "WEIGHTS_0");
                b32 indices_is_valid  = json_child_value(parser->arena, primitives, JSON_VALUE_TYPE_i32, &indices, "indices");
                b32 material_is_valid = json_child_value(parser->arena, mesh, JSON_VALUE_TYPE_i32, &material, "material");
                b32 mode_is_valid     = json_child_value(parser->arena, mesh, JSON_VALUE_TYPE_i32, &mode, "mode");

                result.meshes[i].first_primitive.position  = position_is_valid ? position : -1;
                result.meshes[i].first_primitive.normal    = normal_is_valid ? normal : -1;
                result.meshes[i].first_primitive.tangent   = tangent_is_valid ? tangent : -1;
                result.meshes[i].first_primitive.texcoord  = texcoord_is_valid ? texcoord : -1;
                result.meshes[i].first_primitive.color     = color_is_valid ? color : -1;
                result.meshes[i].first_primitive.joints    = joints_is_valid ? joints : -1;
                result.meshes[i].first_primitive.weights   = weights_is_valid ? weights : -1;
                result.meshes[i].first_primitive.indices   = indices_is_valid ? indices : -1;
                result.meshes[i].first_primitive.material  = material_is_valid ? material : -1;
                result.meshes[i].first_primitive.draw_mode = mode_is_valid ? mode : -1;

                mesh = mesh->next;
            }
        }
        else if (buffer_is_equal(&access_label, &current->label))
        {
            u32 accessor_count    = json_num_of_children(current);
            result.accessors      = MEMORY_PUSH(parser->arena, gltf_accessor, accessor_count);
            result.accessor_count = accessor_count;

            json_entry* accessor = current->child;
            for (u32 i = 0; i < accessor_count && accessor != NULL; i++)
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
        else if (buffer_is_equal(&views_label, &current->label))
        {
            u32 view_count           = json_num_of_children(current);
            result.buffer_views      = MEMORY_PUSH(parser->arena, gltf_buffer_view, view_count);
            result.buffer_view_count = view_count;

            json_entry* view = current->child;
            for (u32 i = 0; i < view_count && view != NULL; i++)
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
        else if (buffer_is_equal(&buffer_label, &current->label))
        {
            u32 buffer_count    = json_num_of_children(current);
            result.buffers      = MEMORY_PUSH(parser->arena, gltf_buffer, buffer_count);
            result.buffer_count = buffer_count;

            json_entry* buffer = current->child;
            for (u32 i = 0; i < buffer_count && buffer != NULL; i++)
            {
                i32 length;

                b32 length_is_valid = json_child_value(parser->arena, buffer, JSON_VALUE_TYPE_i32, &length, "byteLength");

                result.buffers[i].byte_length = length_is_valid ? length : -1;
            }

            buffer = buffer->next;
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

    result.nodes      = MEMORY_PUSH(parser->arena, scene_node, json_data->node_count);
    result.meshes     = MEMORY_PUSH(parser->arena, mesh, json_data->mesh_count);
    result.node_count = json_data->node_count;
    result.mesh_count = json_data->mesh_count;

    // NOTE(KB): Convert node data to output format
    for (u32 node_idx = 0; node_idx < json_data->node_count; node_idx++)
    {
        gltf_node* node = &json_data->nodes[node_idx];

        result.nodes[node_idx].transform   = node->matrix;
        result.nodes[node_idx].children    = node->children;
        result.nodes[node_idx].child_count = node->child_count;
        result.nodes[node_idx].parent      = node->parent;
        result.nodes[node_idx].mesh_id     = node->mesh;
    }

    // NOTE(KB): Convert mesh data to output format
    for (u32 mesh_idx = 0; mesh_idx < json_data->mesh_count; mesh_idx++)
    {
        i32 acs_pos = json_data->meshes[mesh_idx].first_primitive.position;
        i32 acs_nrm = json_data->meshes[mesh_idx].first_primitive.normal;
        i32 acs_uvs = json_data->meshes[mesh_idx].first_primitive.texcoord;
        i32 acs_ids = json_data->meshes[mesh_idx].first_primitive.indices;

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

        result.meshes[mesh_idx].vertex_count = json_data->accessors[acs_pos].count;
        result.meshes[mesh_idx].index_count  = json_data->accessors[acs_ids].count;

        result.meshes[mesh_idx].vertices = MEMORY_PUSH(parser->arena, vertex, result.meshes[mesh_idx].vertex_count);
        result.meshes[mesh_idx].indices  = MEMORY_PUSH(parser->arena, u32,  result.meshes[mesh_idx].index_count);

        gltf_parse_components(
            (data + data_offset_ids),
            result.meshes[mesh_idx].index_count,
            0,
            sizeof(u32),
            json_data->accessors[acs_ids].component_type,
            json_data->accessors[acs_ids].type,
            result.meshes[mesh_idx].indices
        );

        gltf_parse_components(
            (data + data_offset_pos),
            result.meshes[mesh_idx].vertex_count,
            offsetof(vertex, position),
            sizeof(vertex),
            json_data->accessors[acs_pos].component_type,
            json_data->accessors[acs_pos].type,
            result.meshes[mesh_idx].vertices
        );

        gltf_parse_components(
            (data + data_offset_nrm),
            result.meshes[mesh_idx].vertex_count,
            offsetof(vertex, normal),
            sizeof(vertex),
            json_data->accessors[acs_nrm].component_type,
            json_data->accessors[acs_nrm].type,
            result.meshes[mesh_idx].vertices
        );

        gltf_parse_components(
            (data + data_offset_uvs),
            result.meshes[mesh_idx].vertex_count,
            offsetof(vertex, uv),
            sizeof(vertex),
            json_data->accessors[acs_uvs].component_type,
            json_data->accessors[acs_uvs].type,
            result.meshes[mesh_idx].vertices
        );

        // NOTE(): Manually write to the color value
        for (u32 i = 0; i < result.meshes[mesh_idx].vertex_count; i++)
        {
            result.meshes[mesh_idx].vertices[i].color = (vec3){1.0f, 1.0f, 1.0f};
        }
    }

    return result;
}

void gltf_parse_components(void* source, u32 count, u32 offset, u32 stride, i32 cmp_type, i32 data_type, void* dest)
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

            for (u32 i = 0; i < count; i++)
            {
                for (u32 j = 0; j < data_type_size; j++)
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

            for (u32 i = 0; i < count; i++)
            {
                for (u32 j = 0; j < data_type_size; j++)
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

            for (u32 i = 0; i < count; i++)
            {
                for (u32 j = 0; j < data_type_size; j++)
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

            for (u32 i = 0; i < count; i++)
            {
                for (u32 j = 0; j < data_type_size; j++)
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
