#ifndef BUFFER_H
#define BUFFER_H

typedef struct buffer_t buffer_t;
struct buffer_t
{
    u64 size;
    u8* data;
};

internal b32 buffer_is_valid(buffer_t* buf);
internal b32 buffer_is_valid_idx(buffer_t* buf, u64 idx);
internal b32 buffer_is_equal(buffer_t* lhs, buffer_t* rhs);

internal buffer_t buffer_from_cstr(const c8* cstr);
internal void     cstr_from_buffer(buffer_t* buf, c8* dest);

#define string_t buffer_t
#define string_is_valid(s)        buffer_is_valid(s)
#define string_is_valid_idx(s, i) buffer_is_valid(s, i)
#define string_is_equal(l, r)     buffer_is_equal(l, r)
#define string_from_cstr(c)       buffer_from_cstr(c)
#define cstr_from_string(s, d)    cstr_from_buffer(s, d)

#endif //BUFFER_H
