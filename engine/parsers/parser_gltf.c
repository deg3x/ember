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
            json_entry_t* buffer_views = current;

            EMBER_ASSERT(buffer_views != NULL);

            u32_t view_count    = json_num_of_children(buffer_views);
            result.buffer_views = MEMORY_PUSH(scratch.arena, gltf_buffer_views_t, view_count);

            json_entry_t* view = buffer_views->child;
            for (u32_t i = 0; view != NULL; i++)
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
                result.buffer_views[i].byte_offset = offset_is_valid ? offset : -1;
                result.buffer_views[i].byte_length = length_is_valid ? length : -1;
                result.buffer_views[i].byte_stride = stride_is_valid ? stride : -1;
                result.buffer_views[i].target      = target_is_valid ? target : -1;

                EMBER_ASSERT(i < view_count)

                view = view->next;
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
