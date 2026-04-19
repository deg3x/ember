json_entry* json_parse(u8* data, u64 size, cpu_arena* arena)
{
    json_parser parser;

    parser.arena       = arena;
    parser.position    = 0;
    parser.has_error   = EMBER_FALSE;
    parser.source.data = data;
    parser.source.size = size;

    json_token token   = json_parse_token(&parser);
    json_entry* result = json_parse_entry(&parser, &(buffer){0}, &token);

    return result;
}

json_entry* json_parse_file(const c8* file_path, cpu_arena* arena)
{
    platform_hnd file_handle     = platform_file_open(file_path, PLATFORM_FILE_FLAGS_read | PLATFORM_FILE_FLAGS_share_r);
    platform_file_info file_info = platform_file_info_get(file_handle);

    EMBER_ASSERT(file_info.size <= cpu_arena_avail(arena));

    json_parser parser;

    parser.arena       = arena;
    parser.position    = 0;
    parser.has_error   = EMBER_FALSE;
    parser.source.data = MEMORY_PUSH(arena, u8, file_info.size);
    parser.source.size = platform_file_read(file_handle, parser.source.data, FILE_READ_ALL);

    json_token token   = json_parse_token(&parser);
    json_entry* result = json_parse_entry(&parser, &(buffer){0}, &token);

    platform_file_close(file_handle);

    return result;
}

json_entry* json_parse_entry(json_parser* parser, buffer* label, json_token* token)
{
    b32 is_valid = EMBER_TRUE;

    json_entry* sub_entry = NULL;
    if (token->type == JSON_TOKEN_TYPE_bracket_open)
    {
        sub_entry = json_parse_list(parser, JSON_TOKEN_TYPE_bracket_close, EMBER_FALSE);
    }
    else if (token->type == JSON_TOKEN_TYPE_brace_open)
    {
        sub_entry = json_parse_list(parser, JSON_TOKEN_TYPE_brace_close, EMBER_TRUE);
    }
    else if (token->type == JSON_TOKEN_TYPE_string ||
             token->type == JSON_TOKEN_TYPE_number ||
             token->type == JSON_TOKEN_TYPE_true   ||
             token->type == JSON_TOKEN_TYPE_false  ||
             token->type == JSON_TOKEN_TYPE_null)
    {

    }
    else
    {
        is_valid = EMBER_FALSE;
    }

    json_entry* result = NULL;

    if (is_valid)
    {
        result        = MEMORY_PUSH(parser->arena, json_entry, 1);
        result->label = *label;
        result->value = token->value;
        result->child = sub_entry;
        result->next  = NULL;
    }

    return result;
}

json_entry* json_parse_list(json_parser* parser, json_token_type end_type, b32 has_labels)
{
    json_entry* first = NULL;
    json_entry* last  = NULL;

    while (json_parser_is_valid(parser))
    {
        buffer label     = {0};
        json_token token = json_parse_token(parser);

        if (has_labels)
        {
            if (token.type == JSON_TOKEN_TYPE_string)
            {
                label = token.value;

                json_token colon = json_parse_token(parser);
                if (colon.type == JSON_TOKEN_TYPE_colon)
                {
                    token = json_parse_token(parser);
                }
                else
                {
                    json_parser_error(parser);
                }
            }
            else if (token.type != end_type)
            {
                json_parser_error(parser);
            }
        }

        json_entry* entry = json_parse_entry(parser, &label, &token);
        if (entry != NULL)
        {
            if (last != NULL)
            {
                last->next = entry;
                last       = entry;
            }
            else
            {
                first = entry;
                last  = entry;
            }
        }
        else if (token.type == end_type)
        {
            break;
        }
        else
        {
            json_parser_error(parser);
        }

        json_token comma = json_parse_token(parser);
        if (comma.type == end_type)
        {
            break;
        }
        else if (comma.type != JSON_TOKEN_TYPE_comma)
        {
            json_parser_error(parser);
        }
    }

    return first;
}

json_token json_parse_token(json_parser* parser)
{
    json_token result = {0};

    buffer src = parser->source;
    u64 pos    = parser->position;

    while (json_is_whitespace(&src, pos))
    {
        pos += 1;
    }

    if (buffer_is_valid_idx(&src, pos))
    {
        result.type       = JSON_TOKEN_TYPE_error;
        result.value.size = 1;
        result.value.data = src.data + pos;

        u8 byte = src.data[pos];

        switch (byte)
        {
            case '[':
            {
                result.type = JSON_TOKEN_TYPE_bracket_open;
                pos        += 1;

                break;
            }
            case ']':
            {
                result.type = JSON_TOKEN_TYPE_bracket_close;
                pos        += 1;

                break;
            }
            case '{':
            {
                result.type = JSON_TOKEN_TYPE_brace_open;
                pos        += 1;

                break;
            }
            case '}':
            {
                result.type = JSON_TOKEN_TYPE_brace_close;
                pos        += 1;

                break;
            }
            case ',':
            {
                result.type = JSON_TOKEN_TYPE_comma;
                pos        += 1;

                break;
            }
            case ':':
            {
                result.type = JSON_TOKEN_TYPE_colon;
                pos        += 1;

                break;
            }
            case 'f':
            {
                pos += 1;

                buffer remain = buffer_from_cstr("alse");

                if ((src.size - pos) >= remain.size)
                {
                    buffer check = {
                        remain.size,
                        src.data + pos
                    };

                    if (buffer_is_equal(&check, &remain))
                    {
                        result.type = JSON_TOKEN_TYPE_false;
                        result.value.size = remain.size;

                        pos += remain.size;
                    }
                }

                break;
            }
            case 't':
            {
                pos += 1;

                buffer remain = buffer_from_cstr("rue");

                if ((src.size - pos) >= remain.size)
                {
                    buffer check = {
                        remain.size,
                        src.data + pos
                    };

                    if (buffer_is_equal(&check, &remain))
                    {
                        result.type = JSON_TOKEN_TYPE_true;
                        result.value.size = remain.size;

                        pos += remain.size;
                    }
                }

                break;
            }
            case 'n':
            {
                pos += 1;

                buffer remain = buffer_from_cstr("ull");

                if ((src.size - pos) >= remain.size)
                {
                    buffer check = {
                        remain.size,
                        src.data + pos
                    };

                    if (buffer_is_equal(&check, &remain))
                    {
                        result.type = JSON_TOKEN_TYPE_null;
                        result.value.size = remain.size;

                        pos += remain.size;
                    }
                }

                break;
            }
            case '"':
            {
                result.type = JSON_TOKEN_TYPE_string;

                pos += 1;
                u64 start_pos = pos;

                while (buffer_is_valid_idx(&src, pos) && src.data[pos] != '"')
                {
                    if (buffer_is_valid_idx(&src, pos + 1) && src.data[pos] == '\\' && src.data[pos + 1] == '"')
                    {
                        pos += 1;
                    }

                    pos += 1;
                }

                result.value.data = src.data + start_pos;
                result.value.size = pos - start_pos;

                if (buffer_is_valid_idx(&src, pos))
                {
                    pos += 1;
                }

                break;
            }
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                result.type = JSON_TOKEN_TYPE_number;
                u64 start = pos;

                pos += 1;

                if (byte == '-' && buffer_is_valid_idx(&src, pos))
                {
                    byte = src.data[pos];
                    pos += 1;
                }

                if (byte != '0')
                {
                    while (json_is_number(&src, pos))
                    {
                        pos += 1;
                    }
                }

                if (buffer_is_valid_idx(&src, pos) && src.data[pos] == '.')
                {
                    pos += 1;

                    while (json_is_number(&src, pos))
                    {
                        pos += 1;
                    }
                }
                if (buffer_is_valid_idx(&src, pos) && ((src.data[pos] == 'e') || (src.data[pos] == 'E')))
                {
                    pos += 1;

                    if (buffer_is_valid_idx(&src, pos) && ((src.data[pos] == '-') || (src.data[pos] == '+')))
                    {
                        pos += 1;
                    }

                    while (json_is_number(&src, pos))
                    {
                        pos += 1;
                    }
                }

                result.value.size = pos - start;

                break;
            }
            default:
            {
                EMBER_ASSERT(EMBER_FALSE);
                break;
            }
        }
    }

    parser->position = pos;

    return result;
}

i32 json_num_of_children(json_entry* entry)
{
    EMBER_ASSERT(entry != NULL);

    i32 result = 0;

    json_entry* current = entry->child;
    while (current != NULL)
    {
        result += 1;
        current = current->next;
    }

    return result;
}

json_entry* json_find_child(json_entry* entry, buffer* child_label)
{
    EMBER_ASSERT(entry != NULL);

    json_entry* result = NULL;

    for (json_entry* iter = entry->child; iter != NULL; iter = iter->next)
    {
        if (buffer_is_equal(&iter->label, child_label))
        {
            result = iter;
            break;
        }
    }

    return result;
}

b32 json_child_value(cpu_arena* arena, json_entry* entry, json_value_type type, void* dest, const c8* child_label)
{
    EMBER_ASSERT(entry != NULL);
    EMBER_ASSERT(dest != NULL);
    EMBER_ASSERT(child_label != NULL);

    buffer label_buffer = buffer_from_cstr(child_label);
    json_entry* child   = json_find_child(entry, &label_buffer);

    if (child == NULL)
    {
        return EMBER_FALSE;
    }

    cpu_scratch scratch = cpu_scratch_begin(arena);
    b32 result          = EMBER_TRUE;

    switch (type)
    {
        case JSON_VALUE_TYPE_i32:
        {
            c8* value_str = MEMORY_PUSH(scratch.arena, c8, child->value.size + 1);
            buffer_to_cstr(&child->value, value_str);

            *((i32 *)dest) = strtol(value_str, NULL, 10);

            break;
        }
        case JSON_VALUE_TYPE_u32:
        {
            c8* value_str = MEMORY_PUSH(scratch.arena, c8, child->value.size + 1);
            buffer_to_cstr(&child->value, value_str);

            *((u32 *)dest) = strtol(value_str, NULL, 10);

            break;
        }
        case JSON_VALUE_TYPE_f32:
        {
            c8* value_str = MEMORY_PUSH(scratch.arena, c8, child->value.size + 1);
            buffer_to_cstr(&child->value, value_str);

            *((f32 *)dest) = strtof(value_str, NULL);
            
            break;
        }
        case JSON_VALUE_TYPE_b32:
        {
            buffer buf_true  = buffer_from_cstr("tru");
            buffer buf_false = buffer_from_cstr("fals");

            if (buffer_is_equal(&buf_true, &child->value))
            {
                *((b32 *)dest) = EMBER_TRUE;
            }
            else if (buffer_is_equal(&buf_false, &child->value))
            {
                *((b32 *)dest) = EMBER_FALSE;
            }
            else
            {
                EMBER_ASSERT(EMBER_FALSE);
            }

            break;
        }
        case JSON_VALUE_TYPE_str:
        {
            buffer_to_cstr(&child->value, (c8 *)dest);

            break;
        }
        case JSON_VALUE_TYPE_arr_i32:
        {
            if (child->value.size != 1 || *((c8*)(child->value.data)) != '[')
            {
                result = EMBER_FALSE;

                break;
            }

            i32* i_dest = (i32 *)dest;

            json_entry* array_child = child->child;
            while (array_child != NULL)
            {
                c8* value_str = MEMORY_PUSH(scratch.arena, c8, array_child->value.size + 1);
                buffer_to_cstr(&array_child->value, value_str);

                *i_dest = strtol(value_str, NULL, 10);

                i_dest++;

                array_child = array_child->next;
            }

            break;
        }
        case JSON_VALUE_TYPE_arr_f32:
        {
            if (child->value.size != 1 || *((c8*)(child->value.data)) != '[')
            {
                result = EMBER_FALSE;

                break;
            }

            f32* f_dest = (f32 *)dest;

            json_entry* array_child = child->child;
            while (array_child != NULL)
            {
                c8* value_str = MEMORY_PUSH(scratch.arena, c8, array_child->value.size + 1);
                buffer_to_cstr(&array_child->value, value_str);

                *f_dest = strtof(value_str, NULL);

                f_dest++;

                array_child = array_child->next;
            }

            break;
        }
        default:
        {
            result = EMBER_FALSE;

            break;
        }
    }

    cpu_scratch_end(scratch);

    return result;
}

b32 json_parser_is_valid(json_parser* parser)
{
    b32 result = (!parser->has_error && buffer_is_valid_idx(&parser->source, parser->position));

    return result;
}

void json_parser_error(json_parser* parser)
{
    parser->has_error = EMBER_TRUE;
}

b32 json_is_whitespace(buffer* buffer, u64 pos)
{
    b32 result = EMBER_FALSE;

    if (buffer_is_valid_idx(buffer, pos))
    {
        u8 value = buffer->data[pos];

        result = IS_WHITESPACE(value);
    }

    return result;
}

b32 json_is_number(buffer* buffer, u64 pos)
{
    b32 result = EMBER_FALSE;

    if (buffer_is_valid_idx(buffer, pos))
    {
        u8 value = buffer->data[pos];

        result = ((value >= '0') && (value <= '9'));
    }

    return result;
}


