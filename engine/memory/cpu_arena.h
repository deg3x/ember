#ifndef ARENA_H
#define ARENA_H

#define ARENA_HEADER_SIZE 64 

typedef u64 cpu_arena_flags;
enum
{
    CPU_ARENA_FLAGS_large_pages = (1 << 0),
};

typedef struct cpu_arena_params cpu_arena_params;
struct cpu_arena_params
{
    u64             size_res;
    u64             size_cmt;
    cpu_arena_flags flags;
};

typedef struct cpu_arena cpu_arena;
struct cpu_arena
{
    u64             size_res;
    u64             size_cmt;
    u64             position;
    cpu_arena_flags flags;
};

typedef struct cpu_scratch cpu_scratch;
struct cpu_scratch
{
    cpu_arena* arena;
    u64        position;
};

internal cpu_arena* cpu_arena_init(cpu_arena_params* params);
internal void*      cpu_arena_push(cpu_arena* arena, u64 size, u64 align);
internal void       cpu_arena_pop(cpu_arena* arena, u64 size);
internal void       cpu_arena_pop_to(cpu_arena* arena, u64 pos);
internal u64        cpu_arena_avail(cpu_arena* arena);
internal void       cpu_arena_clear(cpu_arena* arena);
internal void       cpu_arena_release(cpu_arena* arena);

internal cpu_scratch cpu_scratch_begin(cpu_arena* arena);
internal void        cpu_scratch_end(cpu_scratch scratch);

#define MEMORY_PUSH(a, t, c)      (t *)cpu_arena_push((a), sizeof(t) * (c), ALIGN_OF(t))
#define MEMORY_PUSH_ZERO(a, t, c) (t *)MEMORY_ZERO(MEMORY_PUSH(a, t, c), sizeof(t) * c)

#endif // ARENA_H
