#ifndef ARENA_H
#define ARENA_H

#define ARENA_HEADER_SIZE 128 

typedef u64 cpu_arena_flags_t;
enum
{
    CPU_ARENA_FLAGS_large_pages = (1 << 0),
    CPU_ARENA_FLAGS_no_chain    = (1 << 1),
};

typedef struct cpu_arena_params_t cpu_arena_params_t;
struct cpu_arena_params_t
{
    c8*               name;
    u64               size_res;
    u64               size_cmt;
    void*             base_memory;
    cpu_arena_flags_t flags;
};

typedef struct cpu_arena_t cpu_arena_t;
struct cpu_arena_t
{
    c8*               name;
    u64               pos_base;
    u64               pos_curr;
    u64               reserved;
    u64               committed;
    u64               size_res;
    u64               size_cmt;
    cpu_arena_t*      previous;
    cpu_arena_t*      current;
    cpu_arena_flags_t flags;
};

EMBER_ASSERT_STATIC(ARENA_HEADER_SIZE >= sizeof(cpu_arena_t), arena_header_size_assert);

typedef struct cpu_scratch_t cpu_scratch_t;
struct cpu_scratch_t
{
    cpu_arena_t* arena;
    u64          position;
};

internal cpu_arena_t* cpu_arena_init(cpu_arena_params_t* params);
internal void*        cpu_arena_push(cpu_arena_t* arena, u64 size, u64 align);
internal void         cpu_arena_pop(cpu_arena_t* arena, u64 size);
internal void         cpu_arena_pop_to(cpu_arena_t* arena, u64 pos);
internal u64          cpu_arena_avail(cpu_arena_t* arena);
internal void         cpu_arena_clear(cpu_arena_t* arena);
internal void         cpu_arena_release(cpu_arena_t* arena);

internal cpu_scratch_t cpu_scratch_begin(cpu_arena_t* arena);
internal void          cpu_scratch_end(cpu_scratch_t scratch);

#define MEMORY_PUSH(a, t, c)      (t *)cpu_arena_push((a), sizeof(t) * (c), ALIGN_OF(t))
#define MEMORY_PUSH_ZERO(a, t, c) (t *)MEMORY_ZERO(MEMORY_PUSH(a, t, c), sizeof(t) * c)

#endif // ARENA_H
