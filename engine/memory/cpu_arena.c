internal cpu_arena_t* cpu_arena_init(cpu_arena_params_t* params)
{
    EMBER_ASSERT(params->size_res >= params->size_cmt);
    EMBER_ASSERT(params->size_cmt >= ARENA_HEADER_SIZE);

    b64 lrg_pages = params->flags & CPU_ARENA_FLAGS_large_pages; 
    u64 page_size = lrg_pages ? g_platform_info.page_size_large : g_platform_info.page_size;

    u64 reserve = POW_2_ROUND_UP(params->size_res + ARENA_HEADER_SIZE, page_size);
    u64 commit  = POW_2_ROUND_UP(params->size_cmt + ARENA_HEADER_SIZE, page_size);

    void* address = NULL;
    
    // NOTE(KB): Large pages not working currently
    //           We need to attempt enabling them in win32
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
    result->name        = params->name;
    result->pos_base    = 0;
    result->pos_curr    = ARENA_HEADER_SIZE;
    result->reserved    = reserve;
    result->committed   = commit;
    result->size_res    = params->size_res;
    result->size_cmt    = params->size_cmt;
    result->flags       = params->flags;
    result->previous    = NULL;
    result->current     = result;

    return result;
}

internal void* cpu_arena_push(cpu_arena_t* arena, u64 size, u64 align)
{
    EMBER_ASSERT(arena != NULL);

    cpu_arena_t* current = arena->current;

    u64 pos_start = POW_2_ROUND_UP(current->pos_curr, align);
    u64 pos_goal  = POW_2_ROUND_UP(pos_start + size, align);

    EMBER_ASSERT(current->reserved >= pos_goal || !(current->flags & CPU_ARENA_FLAGS_no_chain));

    if (current->reserved < pos_goal)
    {
        cpu_arena_params_t params = {0};
        params.name               = current->name;
        params.size_res           = size > current->size_res ? size : current->size_res;
        params.size_cmt           = size > current->size_res ? size : current->size_cmt;
        params.flags              = current->flags;

        cpu_arena_t* new_node = cpu_arena_init(&params);

        new_node->pos_base = current->pos_base + current->reserved;
        new_node->previous = arena->current;
        arena->current     = new_node;

        current = new_node;

        pos_start = POW_2_ROUND_UP(new_node->pos_curr, align);
        pos_goal  = POW_2_ROUND_UP(pos_start + size, align);
    }

    if (current->committed < pos_goal)
    {
        u64 commit = MIN(pos_goal, current->reserved);
        commit    -= current->size_cmt;

        u8* commit_addr = (u8*)current + current->committed;

        if (current->flags & CPU_ARENA_FLAGS_large_pages)
        {
            commit = POW_2_ROUND_UP(commit, g_platform_info.page_size_large);
            platform_mem_commit_large(commit_addr, commit);
        }
        else
        {
            commit = POW_2_ROUND_UP(commit, g_platform_info.page_size);
            platform_mem_commit(commit_addr, commit);
        }

        EMBER_ASSERT((commit + current->committed) <= current->reserved);

        current->committed = current->committed + commit;
    }

    u8* result        = (u8*)current + current->pos_curr;
    current->pos_curr = pos_goal;

    return result;
}

internal void cpu_arena_pop(cpu_arena_t* arena, u64 size)
{
    u64 pos = arena->current->pos_base + arena->current->pos_curr;

    cpu_arena_pop_to(arena, (pos > size) ? (pos - size) : 0);
}

internal void cpu_arena_pop_to(cpu_arena_t* arena, u64 pos)
{
    u64 min_pos = MAX(ARENA_HEADER_SIZE, pos);

    cpu_arena_t* curr = arena->current;
    cpu_arena_t* prev = NULL;

    while (curr->pos_base > min_pos)
    {
        prev = curr->previous;

        platform_mem_release(curr, curr->reserved);

        curr = prev;
    }

    EMBER_ASSERT((min_pos - curr->pos_base) <= curr->pos_curr);

    arena->current = curr;
    curr->pos_curr = min_pos - curr->pos_base;
}

internal u64 cpu_arena_avail(cpu_arena_t* arena)
{
    u64 result = arena->committed - arena->pos_curr;

    return result;
}

internal void cpu_arena_clear(cpu_arena_t* arena)
{
    cpu_arena_pop_to(arena, 0);
}

internal void cpu_arena_release(cpu_arena_t* arena)
{
    cpu_arena_t* curr = arena->current;
    cpu_arena_t* prev = NULL;

    while (curr != NULL)
    {
        prev = curr->previous;

        platform_mem_release(curr, curr->reserved);

        curr = prev;
    }
}

internal cpu_scratch_t cpu_scratch_begin(cpu_arena_t* arena)
{
    cpu_scratch_t scratch = {
        arena,
        arena->current->pos_base + arena->current->pos_curr
    };

    return scratch;
}

internal void cpu_scratch_end(cpu_scratch_t scratch)
{
    cpu_arena_pop_to(scratch.arena, scratch.position);
}
