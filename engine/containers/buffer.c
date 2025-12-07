internal b32_t
buffer_is_valid(buffer_t* buffer)
{
    b32_t result = (buffer->data != NULL);

    return result;
}

internal b32_t
buffer_is_valid_idx(buffer_t* buffer, u64_t idx)
{
    b32_t result = (buffer->size > idx);

    return result;
}

internal b32_t
buffer_is_equal(buffer_t* lhs, buffer_t* rhs)
{
    if (lhs->size != rhs->size)
    {
        return EMBER_FALSE;
    }

    for (u64_t i = 0; i < lhs->size; i++)
    {
        if (lhs->data[i] != rhs->data[i])
        {
            return EMBER_FALSE;
        }
    }

    return EMBER_TRUE;
}

internal buffer_t
buffer_from_cstr(const char* cstr)
{
    buffer_t result = {
        strlen(cstr),
        (u8_t *)cstr
    };

    return result;
}

internal void
buffer_to_cstr(buffer_t* buffer, char* dest)
{
    EMBER_ASSERT(dest != NULL);

    for (i32_t i = 0; i < buffer->size; i++)
    {
        dest[i] = buffer->data[i];
    }

    dest[buffer->size] = '\n';
}
