internal buffer_t buffer_init(buffer_params_t* params)
{
    EMBER_ASSERT(params != NULL);
    EMBER_ASSERT(params->base_capacity > 0);
    EMBER_ASSERT(params->element_size > 0);

    buffer_t result;

    result.arena        = params->base_arena;
    result.element_size = params->element_size;
    result.size         = 0;
    result.capacity     = params->base_capacity;

    i64 init_size = params->base_capacity * params->element_size;

    if (result.arena == NULL)
    {
        // TODO(KB): The buffer will not work if we chain arenas
        //           The addresses will not be continuous anymore and we cannot index elements properly
        //           Fix !!!

        cpu_arena_params_t arena_params = {0};
        arena_params.name               = "Buffer";
        arena_params.size_res           = init_size;
        arena_params.size_cmt           = init_size;
        arena_params.flags              = CPU_ARENA_FLAGS_no_chain;

        result.arena = cpu_arena_init(&arena_params);
    }

    result.data = cpu_arena_push(result.arena, init_size, params->element_size);

    return result;
}

internal void buffer_add(buffer_t* buffer, void* elements, i64 count)
{
    i64 post_cap = buffer->size + count;

    if (buffer->capacity < post_cap)
    {
        i64 mem_req = (post_cap - buffer->capacity) * buffer->element_size;

        void* unused = cpu_arena_push(buffer->arena, mem_req, buffer->element_size);
        (void)unused;

        buffer->capacity = post_cap;
    }

    u8* dst = (u8*)buffer->data + buffer->element_size * buffer->size;
    memcpy(dst, elements, buffer->element_size * count);

    buffer->size += count;
}

internal void buffer_remove(buffer_t* buffer, i64 idx, i64 count)
{
    if (!buffer_is_valid_idx(buffer, idx))
    {
        return;
    }

    EMBER_ASSERT(idx + count <= buffer->size);

    i64 move_count = buffer->size - (idx + count);

    u8* src = (u8*)buffer->data + buffer->element_size * (idx + count);
    u8* dst = (u8*)buffer->data + buffer->element_size * idx;

    memmove(dst, src, buffer->element_size * move_count);

    buffer->size -= count;
}

internal void buffer_remove_unordered(buffer_t* buffer, i64 idx, i64 count)
{
    if (!buffer_is_valid_idx(buffer, idx))
    {
        return;
    }

    EMBER_ASSERT(idx + count <= buffer->size);

    i64 copy_count = buffer->size - (idx + count);

    if (copy_count > count)
    {
        copy_count = count;
    }
    
    if (copy_count > 0)
    {
        i64 copy_idx = buffer->size - copy_count;

        u8* src = (u8*)buffer->data + buffer->element_size * copy_idx;
        u8* dst = (u8*)buffer->data + buffer->element_size * idx;

        memcpy(dst, src, buffer->element_size * copy_count);
    }

    buffer->size -= count;
}

internal b32 buffer_is_valid(buffer_t* buffer)
{
    b32 result = (buffer->data != NULL);

    return result;
}

internal b32 buffer_is_valid_idx(buffer_t* buffer, i64 idx)
{
    b32 result = (buffer->size > idx);

    return result;
}

internal b32 buffer_is_equal(buffer_t* lhs, buffer_t* rhs)
{
    EMBER_ASSERT(buffer_is_valid(lhs) && buffer_is_valid(rhs));

    if (lhs->size != rhs->size || lhs->element_size != rhs->element_size)
    {
        return EMBER_FALSE;
    }

    i8* lhs_data = (i8*)lhs->data;
    i8* rhs_data = (i8*)rhs->data;

    for (i64 i = 0; i < (lhs->size * lhs->element_size); i++)
    {
        if (lhs_data[i] != rhs_data[i])
        {
            return EMBER_FALSE;
        }
    }

    return EMBER_TRUE;
}

internal void buffer_release(buffer_t* buffer)
{
    cpu_arena_release(buffer->arena);

    buffer->arena        = NULL;
    buffer->data         = NULL;
    buffer->size         = 0;
    buffer->capacity     = 0;
    buffer->element_size = 0;
}
