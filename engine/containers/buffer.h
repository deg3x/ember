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
internal void     buffer_to_cstr(buffer_t* buf, c8* dest);

#endif //BUFFER_H
