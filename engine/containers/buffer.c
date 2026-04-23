internal b32 buffer_is_valid(buffer_t* buf)
{
    b32 result = (buf->data != NULL);

    return result;
}

internal b32 buffer_is_valid_idx(buffer_t* buf, u64 idx)
{
    b32 result = (buf->size > idx);

    return result;
}

internal b32 buffer_is_equal(buffer_t* lhs, buffer_t* rhs)
{
    if (lhs->size != rhs->size)
    {
        return EMBER_FALSE;
    }

    for (u64 i = 0; i < lhs->size; i++)
    {
        if (lhs->data[i] != rhs->data[i])
        {
            return EMBER_FALSE;
        }
    }

    return EMBER_TRUE;
}

internal buffer_t buffer_from_cstr(const c8* cstr)
{
    buffer_t result = {
        strlen(cstr),
        (u8 *)cstr
    };

    return result;
}

internal void buffer_to_cstr(buffer_t* buf, c8* dest)
{
    EMBER_ASSERT(dest != NULL);

    for (i32 i = 0; i < buf->size; i++)
    {
        dest[i] = buf->data[i];
    }

    dest[buf->size] = '\0';
}
