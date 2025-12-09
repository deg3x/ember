internal void
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

    while (parser.position < size)
    {
        u32_t chunk_length = *((u32_t*)(parser.source.data + parser.position));
        u32_t chunk_type   = *((u32_t*)(parser.source.data + parser.position + 4));

        parser.position += 8;

        if (chunk_type == GLTF_CHUNK_JSON)
        {
            gltf_parse_chunk_json(&parser, chunk_length);
        }
        else if (chunk_type == GLTF_CHUNK_BIN)
        {
            gltf_parse_chunk_binary(&parser, chunk_length);
        }
        else
        {
            // ERROR
        }

        parser.position += chunk_length;
    }
}

internal void
gltf_parse_chunk_json(gltf_parser_t* parser, u32_t chunk_length)
{
    json_entry_t* json    = json_parse(parser->source.data + parser->position, chunk_length, parser->arena);
    json_entry_t* current = json->child;

    gltf_json_data_t result = {0};

    scratch_t scratch = arena_scratch_begin(parser->arena);

    while (current != NULL)
    {
        buffer_t meshes_label = buffer_from_cstr("meshes");
        buffer_t access_label = buffer_from_cstr("accessors");
        buffer_t views_label  = buffer_from_cstr("bufferViews");

        if (buffer_is_equal(&meshes_label, &current->label))
        {
            
        }
        else if (buffer_is_equal(&access_label, &current->label))
        {
            
        }
        else if (buffer_is_equal(&views_label, &current->label))
        {
            json_entry_t* views = current;

            EMBER_ASSERT(views != NULL);

            u32_t buffer_view_count = json_num_of_children(views);
            result.buffer_views = MEMORY_PUSH(scratch.arena, gltf_buffer_views_t, buffer_view_count);

            json_entry_t* current_view = views->child;
            for (u32_t i = 0; current_view != NULL; i++)
            {
                buffer_t buf_id_label = buffer_from_cstr("buffer");
                buffer_t offset_label = buffer_from_cstr("byteOffset");
                buffer_t length_label = buffer_from_cstr("byteLength");
                buffer_t stride_label = buffer_from_cstr("byteStride");
                buffer_t target_label = buffer_from_cstr("target");

                json_entry_t* buf_id = json_find_child(current_view, &buf_id_label);
                json_entry_t* offset = json_find_child(current_view, &offset_label);
                json_entry_t* length = json_find_child(current_view, &length_label);
                json_entry_t* stride = json_find_child(current_view, &stride_label);
                json_entry_t* target = json_find_child(current_view, &target_label);

                c8_t* buf_id_str;
                c8_t* offset_str;
                c8_t* length_str;
                c8_t* stride_str;
                c8_t* target_str;

                i32_t buf_id_val;
                i32_t offset_val;
                i32_t length_val;
                i32_t stride_val;
                i32_t target_val;

                if (buf_id != NULL)
                {
                    buf_id_str = MEMORY_PUSH(scratch.arena, c8_t, buf_id->value.size);
                    buffer_to_cstr(&buf_id->value, buf_id_str);

                    buf_id_val = strtol(buf_id_str, NULL, 10);
                }

                if (offset != NULL)
                {
                    offset_str = MEMORY_PUSH(scratch.arena, c8_t, offset->value.size);
                    buffer_to_cstr(&offset->value, offset_str);

                    offset_val = strtol(offset_str, NULL, 10);
                }

                if (length != NULL)
                {
                    length_str = MEMORY_PUSH(scratch.arena, c8_t, length->value.size);
                    buffer_to_cstr(&length->value, length_str);

                    length_val = strtol(length_str, NULL, 10);
                }

                if (stride != NULL)
                {
                    stride_str = MEMORY_PUSH(scratch.arena, c8_t, stride->value.size);
                    buffer_to_cstr(&stride->value, stride_str);

                    stride_val = strtol(stride_str, NULL, 10);
                }

                if (target != NULL)
                {
                    target_str = MEMORY_PUSH(scratch.arena, c8_t, target->value.size);
                    buffer_to_cstr(&target->value, target_str);

                    target_val = strtol(target_str, NULL, 10);
                }

                // TODO(): Implement str to numeric conversions
                result.buffer_views[i].buffer_id   = buf_id != NULL ? buf_id_val : -1;
                result.buffer_views[i].byte_offset = offset != NULL ? offset_val : -1;
                result.buffer_views[i].byte_length = length != NULL ? length_val : -1;
                result.buffer_views[i].byte_stride = stride != NULL ? stride_val : -1;
                result.buffer_views[i].target      = target != NULL ? target_val : -1;

                EMBER_ASSERT(i < buffer_view_count)

                current_view = current_view->next;
            }
        }

        current = current->next;
    }

    arena_scratch_end(scratch);
}

internal void
gltf_parse_chunk_binary(gltf_parser_t* parser, u32_t chunk_length)
{

}
