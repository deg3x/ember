#ifndef BUFFER_H
#define BUFFER_H

typedef struct buffer_params_t buffer_params_t;
struct buffer_params_t
{
    cpu_arena_t* base_arena;
    i64          base_capacity;
    i64          element_size;
};

typedef struct buffer_t buffer_t;
struct buffer_t
{
    cpu_arena_t* arena;
    void*        data;
    i64          size;
    i64          capacity;
    i64          element_size;
};

internal buffer_t buffer_init(buffer_params_t* params);
internal void     buffer_add(buffer_t* buffer, void* element, i64 count);
internal void     buffer_remove(buffer_t* buffer, i64 idx, i64 count);
internal void     buffer_remove_unordered(buffer_t* buffer, i64 idx, i64 count);
internal b32      buffer_is_valid(buffer_t* buffer);
internal b32      buffer_is_valid_idx(buffer_t* buffer, i64 idx);
internal b32      buffer_is_equal(buffer_t* lhs, buffer_t* rhs);
internal void     buffer_release(buffer_t* buffer);

#endif // BUFFER_H
