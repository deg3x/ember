#ifndef PARSER_JSON_H
#define PARSER_JSON_H

typedef u32 json_token_type_t;
enum
{
    JSON_TOKEN_TYPE_brace_open,
    JSON_TOKEN_TYPE_brace_close,
    JSON_TOKEN_TYPE_bracket_open,
    JSON_TOKEN_TYPE_bracket_close,
    JSON_TOKEN_TYPE_comma,
    JSON_TOKEN_TYPE_colon,
    JSON_TOKEN_TYPE_string,
    JSON_TOKEN_TYPE_number,
    JSON_TOKEN_TYPE_true,
    JSON_TOKEN_TYPE_false,
    JSON_TOKEN_TYPE_null,
    JSON_TOKEN_TYPE_error,
};

typedef u32 json_value_type_t;
enum
{
    JSON_VALUE_TYPE_i32,
    JSON_VALUE_TYPE_u32,
    JSON_VALUE_TYPE_f32,
    JSON_VALUE_TYPE_b32,
    JSON_VALUE_TYPE_str,
    JSON_VALUE_TYPE_arr_i32,
    JSON_VALUE_TYPE_arr_f32,
};

typedef struct json_token_t json_token_t;
struct json_token_t
{
    buffer_t          value;
    json_token_type_t type;
};

typedef struct json_entry_t json_entry_t;
struct json_entry_t
{
    buffer_t      label;
    buffer_t      value;
    json_entry_t* next;
    json_entry_t* child;
};

typedef struct json_parser_t json_parser_t;
struct json_parser_t
{
    cpu_arena_t* arena;
    buffer_t     source;
    u64          position;
    b64          has_error;
};

internal json_entry_t* json_parse(u8* data, u64 size, cpu_arena_t* arena);
internal json_entry_t* json_parse_file(const c8* file_path, cpu_arena_t* arena);
internal json_entry_t* json_parse_entry(json_parser_t* parser, buffer_t* label, json_token_t* token);
internal json_entry_t* json_parse_list(json_parser_t* parser, json_token_type_t end_type, b32 has_labels);
internal json_token_t  json_parse_token(json_parser_t* parser);

internal i32           json_num_of_children(json_entry_t* entry);
internal json_entry_t* json_find_child(json_entry_t* entry, buffer_t* child_label);
internal b32           json_child_value(cpu_arena_t* arena, json_entry_t* entry, json_value_type_t type, void* dest, const c8* child_label);

internal b32  json_parser_is_valid(json_parser_t* parser);
internal void json_parser_error(json_parser_t* parser);

internal b32  json_is_whitespace(buffer_t* buffer, u64 pos);
internal b32  json_is_number(buffer_t* buffer, u64 pos);

#endif //PARSER_JSON_H
