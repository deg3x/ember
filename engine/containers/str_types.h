#ifndef STRING_H
#define STRING_H

typedef struct string_t string_t;
struct string_t
{
    c8* data;
    i64 size;
    i64 capacity;
};

internal b32 string_is_valid(string_t* str);
internal b32 string_is_valid_idx(string_t* str, i64 idx);
internal b32 string_is_equal(string_t* lhs, string_t* rhs);

internal string_t string_from_cstr(const c8* cstr);
internal void     cstr_from_string(string_t* str, c8* dest);

#endif //STRING_H
