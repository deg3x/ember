internal cpu_arena_t* cpu_arena_init(cpu_arena_params_t* params)
{
    EMBER_ASSERT(params->size_res >= params->size_cmt);
    EMBER_ASSERT(params->size_cmt >= ARENA_HEADER_SIZE);

    b64 lrg_pages = params->flags & CPU_ARENA_FLAGS_large_pages; 
    u64 page_size = lrg_pages ? g_platform_info.page_size_large : g_platform_info.page_size;

    u64 reserve = POW_2_ROUND_UP(params->size_res + ARENA_HEADER_SIZE, page_size);
    u64 commit  = POW_2_ROUND_UP(params->size_cmt + ARENA_HEADER_SIZE, page_size);

    void* address = NULL;
    
    if (lrg_pages)
    {
        address = platform_mem_reserve_large(reserve);
        platform_mem_commit_large(address, commit);
    }
    else
    {
        address = platform_mem_reserve(reserve);
        platform_mem_commit(address, commit);
    }

    EMBER_ASSERT(address != 0);

    if (address == 0)
    {
        platform_abort(1);
    }

    cpu_arena_t* result = (cpu_arena_t*)address;
    result->size_res    = reserve;
    result->size_cmt    = commit;
    result->position    = ARENA_HEADER_SIZE;
    result->flags       = params->flags;

    return result;
}

internal void* cpu_arena_push(cpu_arena_t* arena, u64 size, u64 align)
{
    u64 pos_start = POW_2_ROUND_UP(arena->position, align);
    u64 pos_goal  = POW_2_ROUND_UP(pos_start + size, align);

    EMBER_ASSERT(arena->size_res >= pos_goal);

    if (arena->size_cmt < pos_goal)
    {
        u64 commit = CLAMP_TOP(pos_goal, arena->size_res);
        commit    -= arena->size_cmt;

        if (arena->flags & CPU_ARENA_FLAGS_large_pages)
        {
            commit = POW_2_ROUND_UP(commit, g_platform_info.page_size_large);
            platform_mem_commit_large(arena, commit);
        }
        else
        {
            commit = POW_2_ROUND_UP(commit, g_platform_info.page_size);
            platform_mem_commit(arena, commit);
        }

        pos_goal        = commit;
        arena->size_cmt = commit;
    }

    u8* result      = (u8*)arena + arena->position;
    arena->position = pos_goal;

    return result;
}

internal void cpu_arena_pop(cpu_arena_t* arena, u64 size)
{
    EMBER_ASSERT(arena->position >= size);

    arena->position -= size;
}

internal void cpu_arena_pop_to(cpu_arena_t* arena, u64 pos)
{
    EMBER_ASSERT(arena->position >= pos);

    arena->position = pos;
}

internal u64 cpu_arena_avail(cpu_arena_t* arena)
{
    u64 result = arena->size_cmt - arena->position;

    return result;
}

internal void cpu_arena_clear(cpu_arena_t* arena)
{
    cpu_arena_pop_to(arena, 0);
}

internal void cpu_arena_release(cpu_arena_t* arena)
{
    platform_mem_release(arena, arena->size_res);
}

internal cpu_scratch_t cpu_scratch_begin(cpu_arena_t* arena)
{
    cpu_scratch_t scratch = {
        arena,
        arena->position
    };

    return scratch;
}

internal void cpu_scratch_end(cpu_scratch_t scratch)
{
    cpu_arena_pop_to(scratch.arena, scratch.position);
}
