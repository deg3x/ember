internal json_entry_t*
json_parse(u8_t* data, u64_t size, arena_t* arena)
{
    json_parser_t parser;

    parser.arena       = arena;
    parser.position    = 0;
    parser.has_error   = EMBER_FALSE;
    parser.source.data = data;
    parser.source.size = size;

    json_token_t token   = json_parse_token(&parser);
    json_entry_t* result = json_parse_entry(&parser, &(buffer_t){0}, &token);

    return result;
}

internal json_entry_t*
json_parse_file(const char* file_path, arena_t* arena)
{
    platform_handle_t file_handle    = platform_file_open(file_path, PLATFORM_FILE_FLAGS_read | PLATFORM_FILE_FLAGS_share_r);
    platform_file_props_t file_props = platform_file_props(file_handle);

    EMBER_ASSERT(file_props.size <= arena_avail(arena));

    json_parser_t parser;

    parser.arena       = arena;
    parser.position    = 0;
    parser.has_error   = EMBER_FALSE;
    parser.source.data = MEMORY_PUSH(arena, u8_t, file_props.size);
    parser.source.size = platform_file_read(file_handle, parser.source.data, FILE_READ_ALL);

    json_token_t token   = json_parse_token(&parser);
    json_entry_t* result = json_parse_entry(&parser, &(buffer_t){0}, &token);

    platform_file_close(file_handle);

    return result;
}

internal json_entry_t*
json_parse_entry(json_parser_t* parser, buffer_t* label, json_token_t* token)
{
    b32_t is_valid = EMBER_TRUE;

    json_entry_t* sub_entry = NULL;
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

    json_entry_t* result = NULL;

    if (is_valid)
    {
        result        = MEMORY_PUSH(parser->arena, json_entry_t, 1);
        result->label = *label;
        result->value = token->value;
        result->child = sub_entry;
        result->next  = NULL;
    }

    return result;
}

internal json_entry_t*
json_parse_list(json_parser_t* parser, json_token_type_t end_type, b32_t has_labels)
{
    json_entry_t* first = NULL;
    json_entry_t* last  = NULL;

    while (json_parser_is_valid(parser))
    {
        buffer_t label     = {0};
        json_token_t token = json_parse_token(parser);

        if (has_labels)
        {
            if (token.type == JSON_TOKEN_TYPE_string)
            {
                label = token.value;

                json_token_t colon = json_parse_token(parser);
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

        json_entry_t* entry = json_parse_entry(parser, &label, &token);
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

        json_token_t comma = json_parse_token(parser);
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

internal json_token_t
json_parse_token(json_parser_t* parser)
{
    json_token_t result = {0};

    buffer_t src = parser->source;
    u64_t pos    = parser->position;

    while (json_is_whitespace(&src, pos))
    {
        pos += 1;
    }

    if (buffer_is_valid_idx(&src, pos))
    {
        result.type       = JSON_TOKEN_TYPE_error;
        result.value.size = 1;
        result.value.data = src.data + pos;

        u8_t byte = src.data[pos];

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

                buffer_t remain = buffer_from_cstr("alse");

                if ((src.size - pos) >= remain.size)
                {
                    buffer_t check = {
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

                buffer_t remain = buffer_from_cstr("rue");

                if ((src.size - pos) >= remain.size)
                {
                    buffer_t check = {
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

                buffer_t remain = buffer_from_cstr("ull");

                if ((src.size - pos) >= remain.size)
                {
                    buffer_t check = {
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
                u64_t start_pos = pos;

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
                u64_t start = pos;

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

internal u32_t
json_num_of_children(json_entry_t* entry)
{
    EMBER_ASSERT(entry != NULL);

    u32_t result = 0;

    json_entry_t* current = entry->child;
    while (current != NULL)
    {
        result += 1;
        current = current->next;
    }

    return result;
}

internal json_entry_t*
json_find_child(json_entry_t* entry, buffer_t* child_label)
{
    EMBER_ASSERT(entry != NULL);

    json_entry_t* result = NULL;

    for (json_entry_t* iter = entry->child; iter != NULL; iter = iter->next)
    {
        if (buffer_is_equal(&iter->label, child_label))
        {
            result = iter;
            break;
        }
    }

    return result;
}

internal b32_t
json_child_value(arena_t* arena, json_entry_t* entry, json_value_type_t type, void* dest, const c8_t* child_label)
{
    EMBER_ASSERT(entry != NULL);
    EMBER_ASSERT(dest != NULL);
    EMBER_ASSERT(child_label != NULL);

    buffer_t label_buffer = buffer_from_cstr(child_label);
    json_entry_t* child   = json_find_child(entry, &label_buffer);

    if (child == NULL)
    {
        return EMBER_FALSE;
    }

    scratch_t scratch = arena_scratch_begin(arena);
    b32_t result;

    switch (type)
    {
        case JSON_VALUE_TYPE_i32:
        {
            c8_t* value_str = MEMORY_PUSH(scratch.arena, c8_t, child->value.size);
            buffer_to_cstr(&child->value, value_str);

            *((i32_t *)dest) = strtol(value_str, NULL, 10);

            result = EMBER_TRUE;

            break;
        }
        case JSON_VALUE_TYPE_f32:
        {
            c8_t* value_str = MEMORY_PUSH(scratch.arena, c8_t, child->value.size);
            buffer_to_cstr(&child->value, value_str);

            *((f32_t *)dest) = strtof(value_str, NULL);

            result = EMBER_TRUE;
            
            break;
        }
        default:
        {
            result = EMBER_FALSE;

            break;
        }
    }

    arena_scratch_end(scratch);

    return result;
}

internal b32_t
json_parser_is_valid(json_parser_t* parser)
{
    b32_t result = (!parser->has_error && buffer_is_valid_idx(&parser->source, parser->position));

    return result;
}

internal void
json_parser_error(json_parser_t* parser)
{
    parser->has_error = EMBER_TRUE;
}

internal b32_t
json_is_whitespace(buffer_t* buffer, u64_t pos)
{
    b32_t result = EMBER_FALSE;

    if (buffer_is_valid_idx(buffer, pos))
    {
        u8_t value = buffer->data[pos];

        result = IS_WHITESPACE(value);
    }

    return result;
}

internal b32_t
json_is_number(buffer_t* buffer, u64_t pos)
{
    b32_t result = EMBER_FALSE;

    if (buffer_is_valid_idx(buffer, pos))
    {
        u8_t value = buffer->data[pos];

        result = ((value >= '0') && (value <= '9'));
    }

    return result;
}


