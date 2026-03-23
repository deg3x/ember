#ifndef BUFFER_H
#define BUFFER_H

typedef struct buffer buffer;
struct buffer
{
    u64 size;
    u8* data;
};

b32 buffer_is_valid(buffer* buf);
b32 buffer_is_valid_idx(buffer* buf, u64 idx);
b32 buffer_is_equal(buffer* lhs, buffer* rhs);

buffer buffer_from_cstr(const c8* cstr);
void   buffer_to_cstr(buffer* buf, c8* dest);

#endif //BUFFER_H
