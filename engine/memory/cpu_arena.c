cpu_arena* cpu_arena_init(cpu_arena_params* params)
{
    EMBER_ASSERT(ARENA_HEADER_SIZE >= sizeof(cpu_arena));
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

    cpu_arena* result = (cpu_arena*)address;
    result->size_res  = reserve;
    result->size_cmt  = commit;
    result->position  = ARENA_HEADER_SIZE;
    result->flags     = params->flags;

    return result;
}

void* cpu_arena_push(cpu_arena* arena, u64 size, u64 align)
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

void cpu_arena_pop(cpu_arena* arena, u64 size)
{
    EMBER_ASSERT(arena->position >= size);

    arena->position -= size;
}

void cpu_arena_pop_to(cpu_arena* arena, u64 pos)
{
    EMBER_ASSERT(arena->position >= pos);

    arena->position = pos;
}

u64 cpu_arena_avail(cpu_arena* arena)
{
    u64 result = arena->size_cmt - arena->position;

    return result;
}

void cpu_arena_clear(cpu_arena* arena)
{
    cpu_arena_pop_to(arena, 0);
}

void cpu_arena_release(cpu_arena* arena)
{
    platform_mem_release(arena, arena->size_res);
}

cpu_scratch cpu_scratch_begin(cpu_arena* arena)
{
    cpu_scratch scratch = {
        arena,
        arena->position
    };

    return scratch;
}

void cpu_scratch_end(cpu_scratch scratch)
{
    cpu_arena_pop_to(scratch.arena, scratch.position);
}
