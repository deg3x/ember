internal gltf_data_t
gltf_parse_file(const char* file_path, arena_t* arena)
{
    platform_handle_t file_handle    = platform_file_open(file_path, PLATFORM_FILE_FLAGS_read | PLATFORM_FILE_FLAGS_share_r);
    platform_file_props_t file_props = platform_file_props(file_handle);

    EMBER_ASSERT(file_props.size <= arena_avail(arena));

    gltf_parser_t parser;

    parser.arena       = arena;
    parser.source.data = MEMORY_PUSH(arena, u8_t, file_props.size);
    parser.source.size = platform_file_read(file_handle, parser.source.data, FILE_READ_ALL);
    parser.position    = 0;

    u32_t magic   = *((u32_t*)(parser.source.data));
    u32_t version = *((u32_t*)(parser.source.data + 4));
    u32_t size    = *((u32_t*)(parser.source.data + 8));

    if (magic != GLTF_MAGIC)
    {
        return;
    }

    parser.position = 12;

    gltf_json_data_t json_data = {0};
    gltf_data_t result         = {0};

    while (parser.position < size)
    {
        u32_t chunk_length = *((u32_t*)(parser.source.data + parser.position));
        u32_t chunk_type   = *((u32_t*)(parser.source.data + parser.position + 4));

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

internal gltf_json_data_t
gltf_parse_chunk_json(gltf_parser_t* parser, u32_t chunk_length)
{
    json_entry_t* json    = json_parse(parser->source.data + parser->position, chunk_length, parser->arena);
    json_entry_t* current = json->child;

    gltf_json_data_t result = {0};

    while (current != NULL)
    {
        buffer_t meshes_label = buffer_from_cstr("meshes");
        buffer_t access_label = buffer_from_cstr("accessors");
        buffer_t views_label  = buffer_from_cstr("bufferViews");
        buffer_t buffer_label = buffer_from_cstr("buffers");

        if (buffer_is_equal(&meshes_label, &current->label))
        {
            u32_t mesh_count  = json_num_of_children(current);
            result.meshes     = MEMORY_PUSH(parser->arena, gltf_mesh_t, mesh_count);
            result.mesh_count = mesh_count;

            json_entry_t* mesh = current->child;
            for (u32_t i = 0; i < mesh_count && mesh != NULL; i++)
            {
                i32_t position;
                i32_t normal;
                i32_t tangent;
                i32_t texcoord;
                i32_t color;
                i32_t joints;
                i32_t weights;
                i32_t indices;
                i32_t material;
                i32_t mode;

                // TODO(): Fill missing info, if necessary

                buffer_t primitives_buffer = buffer_from_cstr("primitives");
                buffer_t attributes_buffer = buffer_from_cstr("attributes");

                json_entry_t* primitives = json_find_child(mesh, &primitives_buffer);
                primitives               = primitives->child;

                json_entry_t* attributes = json_find_child(primitives, &attributes_buffer);

                b32_t position_is_valid = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &position, "POSITION");
                b32_t normal_is_valid   = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &normal, "NORMAL");
                b32_t tangent_is_valid  = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &tangent, "TANGENT");
                b32_t texcoord_is_valid = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &texcoord, "TEXCOORD_0");
                b32_t color_is_valid    = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &color, "COLOR_0");
                b32_t joints_is_valid   = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &joints, "JOINTS_0");
                b32_t weights_is_valid  = json_child_value(parser->arena, attributes, JSON_VALUE_TYPE_i32, &weights, "WEIGHTS_0");
                b32_t indices_is_valid  = json_child_value(parser->arena, primitives, JSON_VALUE_TYPE_i32, &indices, "indices");
                b32_t material_is_valid = json_child_value(parser->arena, mesh, JSON_VALUE_TYPE_i32, &material, "material");
                b32_t mode_is_valid     = json_child_value(parser->arena, mesh, JSON_VALUE_TYPE_i32, &mode, "mode");

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
            u32_t accessor_count  = json_num_of_children(current);
            result.accessors      = MEMORY_PUSH(parser->arena, gltf_accessor_t, accessor_count);
            result.accessor_count = accessor_count;

            json_entry_t* accessor = current->child;
            for (u32_t i = 0; i < accessor_count && accessor != NULL; i++)
            {
                i32_t buffer_view_id;
                i32_t byte_offset;
                i32_t component_type;
                i32_t count;
                c8_t type[16];

                // TODO(): Fill missing info, if necessary

                b32_t buffer_view_id_is_valid = json_child_value(parser->arena, accessor, JSON_VALUE_TYPE_i32, &buffer_view_id, "bufferView");
                b32_t byte_offset_is_valid    = json_child_value(parser->arena, accessor, JSON_VALUE_TYPE_i32, &byte_offset, "byteOffset");
                b32_t component_type_is_valid = json_child_value(parser->arena, accessor, JSON_VALUE_TYPE_i32, &component_type, "componentType");
                b32_t count_is_valid          = json_child_value(parser->arena, accessor, JSON_VALUE_TYPE_i32, &count, "count");
                b32_t type_is_valid           = json_child_value(parser->arena, accessor, JSON_VALUE_TYPE_str, &type, "type");

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
            u32_t view_count         = json_num_of_children(current);
            result.buffer_views      = MEMORY_PUSH(parser->arena, gltf_buffer_view_t, view_count);
            result.buffer_view_count = view_count;

            json_entry_t* view = current->child;
            for (u32_t i = 0; i < view_count && view != NULL; i++)
            {
                i32_t buf_id;
                i32_t offset;
                i32_t length;
                i32_t stride;
                i32_t target;

                b32_t buf_id_is_valid = json_child_value(parser->arena, view, JSON_VALUE_TYPE_i32, &buf_id, "buffer");
                b32_t offset_is_valid = json_child_value(parser->arena, view, JSON_VALUE_TYPE_i32, &offset, "byteOffset");
                b32_t length_is_valid = json_child_value(parser->arena, view, JSON_VALUE_TYPE_i32, &length, "byteLength");
                b32_t stride_is_valid = json_child_value(parser->arena, view, JSON_VALUE_TYPE_i32, &stride, "byteStride");
                b32_t target_is_valid = json_child_value(parser->arena, view, JSON_VALUE_TYPE_i32, &target, "target");

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
            u32_t buffer_count  = json_num_of_children(current);
            result.buffers      = MEMORY_PUSH(parser->arena, gltf_buffer_t, buffer_count);
            result.buffer_count = buffer_count;

            json_entry_t* buffer = current->child;
            for (u32_t i = 0; i < buffer_count && buffer != NULL; i++)
            {
                i32_t length;

                b32_t length_is_valid = json_child_value(parser->arena, buffer, JSON_VALUE_TYPE_i32, &length, "byteLength");

                result.buffers[i].byte_length = length_is_valid ? length : -1;
            }

            buffer = buffer->next;
        }

        current = current->next;
    }

    return result;
}

internal gltf_data_t
gltf_parse_chunk_binary(gltf_parser_t* parser, u32_t chunk_length, gltf_json_data_t* json_data)
{
    u8_t* data = parser->source.data + parser->position;

    gltf_data_t result = {0};

    result.meshes     = MEMORY_PUSH(parser->arena, mesh_t, json_data->mesh_count);
    result.mesh_count = json_data->mesh_count;

    EMBER_ASSERT(json_data != NULL);

    for (u32_t mesh_idx = 0; mesh_idx < json_data->mesh_count; mesh_idx++)
    {
        i32_t acs_pos = json_data->meshes[mesh_idx].first_primitive.position;
        i32_t acs_nrm = json_data->meshes[mesh_idx].first_primitive.normal;
        i32_t acs_uvs = json_data->meshes[mesh_idx].first_primitive.texcoord;
        i32_t acs_ids = json_data->meshes[mesh_idx].first_primitive.indices;

        i32_t data_offset_pos = json_data->buffer_views[json_data->accessors[acs_pos].buffer_view_id].byte_offset
                              + json_data->accessors[acs_pos].byte_offset;

        i32_t data_offset_nrm = json_data->buffer_views[json_data->accessors[acs_nrm].buffer_view_id].byte_offset
                              + json_data->accessors[acs_nrm].byte_offset;

        i32_t data_offset_uvs = json_data->buffer_views[json_data->accessors[acs_uvs].buffer_view_id].byte_offset
                              + json_data->accessors[acs_uvs].byte_offset;

        i32_t data_offset_ids = json_data->buffer_views[json_data->accessors[acs_ids].buffer_view_id].byte_offset
                              + json_data->accessors[acs_ids].byte_offset;

        EMBER_ASSERT(
            (json_data->accessors[acs_pos].count == json_data->accessors[acs_nrm].count) &&
            (json_data->accessors[acs_pos].count == json_data->accessors[acs_uvs].count)
        );

        result.meshes[mesh_idx].vertex_count = json_data->accessors[acs_pos].count;
        result.meshes[mesh_idx].index_count  = json_data->accessors[acs_ids].count;

        result.meshes[mesh_idx].vertices = MEMORY_PUSH(parser->arena, vertex_t, result.meshes[mesh_idx].vertex_count);
        result.meshes[mesh_idx].indices  = MEMORY_PUSH(parser->arena, u32_t,  result.meshes[mesh_idx].index_count);

        gltf_parse_components(
            (data + data_offset_ids),
            result.meshes[mesh_idx].index_count,
            0,
            sizeof(u32_t),
            json_data->accessors[acs_ids].component_type,
            json_data->accessors[acs_ids].type,
            result.meshes[mesh_idx].indices
        );

        gltf_parse_components(
            (data + data_offset_pos),
            result.meshes[mesh_idx].vertex_count,
            offsetof(vertex_t, position),
            sizeof(vertex_t),
            json_data->accessors[acs_pos].component_type,
            json_data->accessors[acs_pos].type,
            result.meshes[mesh_idx].vertices
        );

        gltf_parse_components(
            (data + data_offset_nrm),
            result.meshes[mesh_idx].vertex_count,
            offsetof(vertex_t, normal),
            sizeof(vertex_t),
            json_data->accessors[acs_nrm].component_type,
            json_data->accessors[acs_nrm].type,
            result.meshes[mesh_idx].vertices
        );

        gltf_parse_components(
            (data + data_offset_uvs),
            result.meshes[mesh_idx].vertex_count,
            offsetof(vertex_t, uv),
            sizeof(vertex_t),
            json_data->accessors[acs_uvs].component_type,
            json_data->accessors[acs_uvs].type,
            result.meshes[mesh_idx].vertices
        );

        // NOTE(): Manually write to the color value
        for (u32_t i = 0; i < result.meshes[mesh_idx].vertex_count; i++)
        {
            result.meshes[mesh_idx].vertices[i].color = (vec3_t){1.0f, 1.0f, 1.0f};
        }
    }

    return result;
}

internal void
gltf_parse_components(void* source, u32_t count, u32_t offset, u32_t stride, i32_t cmp_type, i32_t data_type, void* dest)
{
    offset /= 4;
    stride /= 4;

    u8_t data_type_size = 1;
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
            u8_t* src_data  = (u8_t *)source;
            u32_t* dst_data = (u32_t *)dest;
            for (u32_t i = 0; i < count; i++)
            {
                for (u32_t j = 0; j < data_type_size; j++)
                {
                    *(dst_data + i * stride + offset + j) = *(src_data + i * data_type_size + j);
                }
            }

            break;
        }
        case GLTF_COMPONENT_TYPE_SHORT:
        case GLTF_COMPONENT_TYPE_USHORT:
        {
            u16_t* src_data = (u16_t *)source;
            u32_t* dst_data = (u32_t *)dest;
            for (u32_t i = 0; i < count; i++)
            {
                for (u32_t j = 0; j < data_type_size; j++)
                {
                    *(dst_data + i * stride + offset + j) = *(src_data + i * data_type_size + j);
                }
            }

            break;
        }
        case GLTF_COMPONENT_TYPE_UINT:
        {
            u32_t* src_data = (u32_t *)source;
            u32_t* dst_data = (u32_t *)dest;
            for (u32_t i = 0; i < count; i++)
            {
                for (u32_t j = 0; j < data_type_size; j++)
                {
                    *(dst_data + i * stride + offset + j) = *(src_data + i * data_type_size + j);
                }
            }

            break;
        }
        case GLTF_COMPONENT_TYPE_FLOAT:
        {
            f32_t* src_data = (f32_t *)source;
            f32_t* dst_data = (f32_t *)dest;
            for (u32_t i = 0; i < count; i++)
            {
                for (u32_t j = 0; j < data_type_size; j++)
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
