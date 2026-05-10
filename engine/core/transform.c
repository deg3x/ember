internal vec3_t transform_fw(transform_t* transform)
{
    vec3_t result = vec3_rotate_quat(&VEC3_FORWARD, &transform->rotation);

    return result;
}

internal vec3_t transform_bk(transform_t* transform)
{
    vec3_t result = vec3_rotate_quat(&VEC3_BACK, &transform->rotation);

    return result;
}

internal vec3_t transform_up(transform_t* transform)
{
    vec3_t result = vec3_rotate_quat(&VEC3_UP, &transform->rotation);

    return result;
}

internal vec3_t transform_dn(transform_t* transform)
{
    vec3_t result = vec3_rotate_quat(&VEC3_DOWN, &transform->rotation);

    return result;
}

internal vec3_t transform_rt(transform_t* transform)
{
    vec3_t result = vec3_rotate_quat(&VEC3_RIGHT, &transform->rotation);

    return result;
}

internal vec3_t transform_lt(transform_t* transform)
{
    vec3_t result = vec3_rotate_quat(&VEC3_LEFT, &transform->rotation);

    return result;
}
