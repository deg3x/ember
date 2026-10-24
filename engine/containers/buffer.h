#ifndef BUFFER_H
#define BUFFER_H

typedef struct buffer_t buffer_t;
struct buffer_t
{
    void* data;
    i64   size;
    i64   capacity;
    i64   element_size;
};

internal b32 buffer_is_valid(buffer_t* buf);
internal b32 buffer_is_valid_idx(buffer_t* buf, i64 idx);
internal b32 buffer_is_equal(buffer_t* lhs, buffer_t* rhs);

#endif //BUFFER_H
