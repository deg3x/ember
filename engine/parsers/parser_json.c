internal json_data_t json_parse(void* data, i64 size)
{
    json_parser_t parser;

    cpu_arena_params_t params = {0};
    params.name               = "JSON Parser";
    params.size_res           = MAX(size * 8, KB(64));
    params.size_cmt           = MAX(size * 2, KB(64));
    params.flags              = 0;

    parser.arena           = cpu_arena_init(&params);
    parser.position        = 0;
    parser.has_error       = EMBER_FALSE;
    parser.source.data     = (c8*)data;
    parser.source.size     = size;
    parser.source.capacity = size;

    json_token_t token = json_parse_token(&parser);
    json_entry_t* root = json_parse_entry(&parser, &(string_t){0}, &token);

    json_data_t result = {0};
    result.arena       = parser.arena;
    result.root        = root;

    return result;
}

internal json_data_t json_parse_file(const c8* file_path)
{
    platform_hnd_t file_handle     = platform_file_open(file_path, PLATFORM_FILE_FLAGS_read | PLATFORM_FILE_FLAGS_share_r);
    platform_file_info_t file_info = platform_file_info_get(file_handle);

    json_parser_t parser;

    cpu_arena_params_t params = {0};
    params.name               = "JSON Parser";
    params.size_res           = MAX(file_info.size * 8, KB(64));
    params.size_cmt           = MAX(file_info.size * 2, KB(64));
    params.flags              = 0;

    parser.arena           = cpu_arena_init(&params);
    parser.position        = 0;
    parser.has_error       = EMBER_FALSE;
    parser.source.data     = MEMORY_PUSH(parser.arena, c8, file_info.size);
    parser.source.size     = platform_file_read(file_handle, parser.source.data, FILE_READ_ALL);
    parser.source.capacity = parser.source.size;

    json_token_t token   = json_parse_token(&parser);
    json_entry_t* root   = json_parse_entry(&parser, &(string_t){0}, &token);

    platform_file_close(file_handle);

    json_data_t result = {0};
    result.arena       = parser.arena;
    result.root        = root;

    return result;
}

internal json_entry_t* json_parse_entry(json_parser_t* parser, string_t* label, json_token_t* token)
{
    b32 is_valid = EMBER_TRUE;

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

internal json_entry_t* json_parse_list(json_parser_t* parser, json_token_type_t end_type, b32 has_labels)
{
    json_entry_t* first = NULL;
    json_entry_t* last  = NULL;

    while (json_parser_is_valid(parser))
    {
        string_t label     = {0};
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

internal json_token_t json_parse_token(json_parser_t* parser)
{
    json_token_t result = {0};

    string_t src = parser->source;
    i64 pos      = parser->position;

    while (json_is_whitespace(&src, pos))
    {
        pos += 1;
    }

    if (string_is_valid_idx(&src, pos))
    {
        result.type           = JSON_TOKEN_TYPE_error;
        result.value.data     = src.data + pos;
        result.value.size     = 1;
        result.value.capacity = 1;

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

                string_t remain = string_from_cstr("alse");

                if ((src.size - pos) >= remain.size)
                {
                    string_t check = {
                        src.data + pos,
                        remain.size,
                        remain.capacity
                    };

                    if (string_is_equal(&check, &remain))
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

                string_t remain = string_from_cstr("rue");

                if ((src.size - pos) >= remain.size)
                {
                    string_t check = {
                        src.data + pos,
                        remain.size,
                        remain.capacity
                    };

                    if (string_is_equal(&check, &remain))
                    {
                        result.type           = JSON_TOKEN_TYPE_true;
                        result.value.size     = remain.size;
                        result.value.capacity = remain.size;

                        pos += remain.size;
                    }
                }

                break;
            }
            case 'n':
            {
                pos += 1;

                string_t remain = string_from_cstr("ull");

                if ((src.size - pos) >= remain.size)
                {
                    string_t check = {
                        src.data + pos,
                        remain.size,
                        remain.capacity
                    };

                    if (string_is_equal(&check, &remain))
                    {
                        result.type           = JSON_TOKEN_TYPE_null;
                        result.value.size     = remain.size;
                        result.value.capacity = remain.size;

                        pos += remain.size;
                    }
                }

                break;
            }
            case '"':
            {
                result.type = JSON_TOKEN_TYPE_string;

                pos += 1;
                i64 start_pos = pos;

                while (string_is_valid_idx(&src, pos) && src.data[pos] != '"')
                {
                    if (string_is_valid_idx(&src, pos + 1) && src.data[pos] == '\\' && src.data[pos + 1] == '"')
                    {
                        pos += 1;
                    }

                    pos += 1;
                }

                result.value.data     = src.data + start_pos;
                result.value.size     = pos - start_pos;
                result.value.capacity = result.value.size;

                if (string_is_valid_idx(&src, pos))
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
                i64 start = pos;

                pos += 1;

                if (byte == '-' && string_is_valid_idx(&src, pos))
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

                if (string_is_valid_idx(&src, pos) && src.data[pos] == '.')
                {
                    pos += 1;

                    while (json_is_number(&src, pos))
                    {
                        pos += 1;
                    }
                }
                if (string_is_valid_idx(&src, pos) && ((src.data[pos] == 'e') || (src.data[pos] == 'E')))
                {
                    pos += 1;

                    if (string_is_valid_idx(&src, pos) && ((src.data[pos] == '-') || (src.data[pos] == '+')))
                    {
                        pos += 1;
                    }

                    while (json_is_number(&src, pos))
                    {
                        pos += 1;
                    }
                }

                result.value.size     = pos - start;
                result.value.capacity = result.value.size;

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

internal json_entry_t* json_child_find(json_entry_t* entry, string_t* child_label)
{
    EMBER_ASSERT(entry != NULL);

    json_entry_t* result = NULL;

    for (json_entry_t* iter = entry->child; iter != NULL; iter = iter->next)
    {
        if (string_is_equal(&iter->label, child_label))
        {
            result = iter;
            break;
        }
    }

    return result;
}

internal i32 json_child_count(json_entry_t* entry)
{
    EMBER_ASSERT(entry != NULL);

    i32 result = 0;

    json_entry_t* current = entry->child;
    while (current != NULL)
    {
        result += 1;
        current = current->next;
    }

    return result;
}

internal b32 json_child_value(cpu_arena_t* arena, json_entry_t* entry, json_value_type_t type, void* dest, const c8* child_label)
{
    EMBER_ASSERT(entry != NULL);
    EMBER_ASSERT(dest != NULL);
    EMBER_ASSERT(child_label != NULL);

    string_t label_string = string_from_cstr(child_label);
    json_entry_t* child   = json_child_find(entry, &label_string);

    if (child == NULL)
    {
        return EMBER_FALSE;
    }

    cpu_scratch_t scratch = cpu_scratch_begin(arena);
    b32 result            = EMBER_TRUE;

    switch (type)
    {
        case JSON_VALUE_TYPE_i32:
        {
            c8* value_str = MEMORY_PUSH(scratch.arena, c8, child->value.size + 1);
            cstr_from_string(&child->value, value_str);

            *((i32 *)dest) = strtol(value_str, NULL, 10);

            break;
        }
        case JSON_VALUE_TYPE_u32:
        {
            c8* value_str = MEMORY_PUSH(scratch.arena, c8, child->value.size + 1);
            cstr_from_string(&child->value, value_str);

            *((u32 *)dest) = strtol(value_str, NULL, 10);

            break;
        }
        case JSON_VALUE_TYPE_f32:
        {
            c8* value_str = MEMORY_PUSH(scratch.arena, c8, child->value.size + 1);
            cstr_from_string(&child->value, value_str);

            *((f32 *)dest) = strtof(value_str, NULL);
            
            break;
        }
        case JSON_VALUE_TYPE_b32:
        {
            string_t buf_true  = string_from_cstr("tru");
            string_t buf_false = string_from_cstr("fals");

            if (string_is_equal(&buf_true, &child->value))
            {
                *((b32 *)dest) = EMBER_TRUE;
            }
            else if (string_is_equal(&buf_false, &child->value))
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
            cstr_from_string(&child->value, (c8 *)dest);

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

            json_entry_t* array_child = child->child;
            while (array_child != NULL)
            {
                c8* value_str = MEMORY_PUSH(scratch.arena, c8, array_child->value.size + 1);
                cstr_from_string(&array_child->value, value_str);

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

            json_entry_t* array_child = child->child;
            while (array_child != NULL)
            {
                c8* value_str = MEMORY_PUSH(scratch.arena, c8, array_child->value.size + 1);
                cstr_from_string(&array_child->value, value_str);

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

internal b32 json_parser_is_valid(json_parser_t* parser)
{
    b32 result = (!parser->has_error && string_is_valid_idx(&parser->source, parser->position));

    return result;
}

internal void json_parser_error(json_parser_t* parser)
{
    parser->has_error = EMBER_TRUE;
}

internal b32 json_is_whitespace(string_t* str, i64 pos)
{
    b32 result = EMBER_FALSE;

    if (string_is_valid_idx(str, pos))
    {
        c8 value = str->data[pos];

        result = IS_WHITESPACE(value);
    }

    return result;
}

internal b32 json_is_number(string_t* str, i64 pos)
{
    b32 result = EMBER_FALSE;

    if (string_is_valid_idx(str, pos))
    {
        c8 value = str->data[pos];

        result = ((value >= '0') && (value <= '9'));
    }

    return result;
}

internal void json_free(json_data_t* json)
{
    cpu_arena_release(json->arena);
}

