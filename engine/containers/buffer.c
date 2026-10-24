internal b32 buffer_is_valid(buffer_t* buf)
{
    b32 result = (buf->data != NULL && buf->capacity >= buf->size);

    return result;
}

internal b32 buffer_is_valid_idx(buffer_t* buf, i64 idx)
{
    b32 result = (buf->size > idx && buf->capacity > idx);

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
