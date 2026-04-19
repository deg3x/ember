#ifndef PARSER_JSON_H
#define PARSER_JSON_H

typedef u32 json_token_type;
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

typedef u32 json_value_type;
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

typedef struct json_token json_token;
struct json_token
{
    buffer          value;
    json_token_type type;
};

typedef struct json_entry json_entry;
struct json_entry
{
    buffer      label;
    buffer      value;
    json_entry* next;
    json_entry* child;
};

typedef struct json_parser json_parser;
struct json_parser
{
    cpu_arena* arena;
    buffer     source;
    u64        position;
    b64        has_error;
};

json_entry* json_parse(u8* data, u64 size, cpu_arena* arena);
json_entry* json_parse_file(const c8* file_path, cpu_arena* arena);
json_entry* json_parse_entry(json_parser* parser, buffer* label, json_token* token);
json_entry* json_parse_list(json_parser* parser, json_token_type end_type, b32 has_labels);
json_token  json_parse_token(json_parser* parser);

i32         json_num_of_children(json_entry* entry);
json_entry* json_find_child(json_entry* entry, buffer* child_label);
b32         json_child_value(cpu_arena* arena, json_entry* entry, json_value_type type, void* dest, const c8* child_label);

b32  json_parser_is_valid(json_parser* parser);
void json_parser_error(json_parser* parser);

b32 json_is_whitespace(buffer* buffer, u64 pos);
b32 json_is_number(buffer* buffer, u64 pos);

#endif //PARSER_JSON_H
