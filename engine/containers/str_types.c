internal b32 string_is_valid(string_t* str)
{
    b32 result = (str->data != NULL && str->capacity >= str->size);

    return result;
}

internal b32 string_is_valid_idx(string_t* str, i64 idx)
{
    b32 result = (str->size > idx && str->capacity > idx);

    return result;
}

internal b32 string_is_equal(string_t* lhs, string_t* rhs)
{
    EMBER_ASSERT(string_is_valid(lhs) && string_is_valid(rhs));

    if (lhs->size != rhs->size)
    {
        return EMBER_FALSE;
    }

    for (i64 i = 0; i < lhs->size; i++)
    {
        if (lhs->data[i] != rhs->data[i])
        {
            return EMBER_FALSE;
        }
    }

    return EMBER_TRUE;
}

internal string_t string_from_cstr(const c8* cstr)
{
    string_t result = {
        (c8 *)cstr,
        strlen(cstr),
        strlen(cstr)
    };

    return result;
}

internal void cstr_from_string(string_t* str, c8* dst)
{
    EMBER_ASSERT(str != NULL);
    EMBER_ASSERT(dst != NULL);

    for (i32 i = 0; i < str->size; i++)
    {
        dst[i] = str->data[i];
    }

    dst[str->size] = '\0';
}
