internal vec2_t vec2_norm(vec2_t* vector)
{
    f32 inv_len = 1.0f / vec2_len(vector);

    vec2_t result = {
        vector->x * inv_len,
        vector->y * inv_len
    };

    return result;
}

internal vec3_t vec3_norm(vec3_t* vector)
{
    f32 inv_len = 1.0f / vec3_len(vector);

    vec3_t result = {
        vector->x * inv_len,
        vector->y * inv_len,
        vector->z * inv_len
    };

    return result;
}

internal vec4_t vec4_norm(vec4_t* vector)
{
    f32 inv_len = 1.0f / vec4_len(vector);

    vec4_t result = {
        vector->x * inv_len,
        vector->y * inv_len,
        vector->z * inv_len,
        vector->w * inv_len
    };

    return result;
}

internal vec2_t vec2_neg(vec2_t* vector)
{
    vec2_t result = {
        -vector->x,
        -vector->y
    };

    return result;
}

internal vec3_t vec3_neg(vec3_t* vector)
{
    vec3_t result = {
        -vector->x,
        -vector->y,
        -vector->z
    };

    return result;
}

internal vec4_t vec4_neg(vec4_t* vector)
{
    vec4_t result = {
        -vector->x,
        -vector->y,
        -vector->z,
        -vector->w
    };

    return result;
}

internal vec2_t vec2_add(vec2_t* lhs, vec2_t* rhs)
{
    vec2_t result = {
        lhs->x + rhs->x,
        lhs->y + rhs->y
    };

    return result;
}

internal vec3_t vec3_add(vec3_t* lhs, vec3_t* rhs)
{
    vec3_t result = {
        lhs->x + rhs->x,
        lhs->y + rhs->y,
        lhs->z + rhs->z
    };

    return result;
}

internal vec4_t vec4_add(vec4_t* lhs, vec4_t* rhs)
{
    vec4_t result = {
        lhs->x + rhs->x,
        lhs->y + rhs->y,
        lhs->z + rhs->z,
        lhs->w + rhs->w
    };

    return result;
}

internal vec2_t vec2_sub(vec2_t* lhs, vec2_t* rhs)
{
    vec2_t result = {
        lhs->x - rhs->x,
        lhs->y - rhs->y
    };

    return result;
}

internal vec3_t vec3_sub(vec3_t* lhs, vec3_t* rhs)
{
    vec3_t result = {
        lhs->x - rhs->x,
        lhs->y - rhs->y,
        lhs->z - rhs->z
    };

    return result;
}

internal vec4_t vec4_sub(vec4_t* lhs, vec4_t* rhs)
{
    vec4_t result = {
        lhs->x - rhs->x,
        lhs->y - rhs->y,
        lhs->z - rhs->z,
        lhs->w - rhs->w
    };

    return result;
}

internal vec2_t vec2_mul(vec2_t* lhs, vec2_t* rhs)
{
    vec2_t result = {
        lhs->x * rhs->x,
        lhs->y * rhs->y
    };

    return result;
}

internal vec3_t vec3_mul(vec3_t* lhs, vec3_t* rhs)
{
    vec3_t result = {
        lhs->x * rhs->x,
        lhs->y * rhs->y,
        lhs->z * rhs->z
    };

    return result;
}

internal vec4_t vec4_mul(vec4_t* lhs, vec4_t* rhs)
{
    vec4_t result = {
        lhs->x * rhs->x,
        lhs->y * rhs->y,
        lhs->z * rhs->z,
        lhs->w * rhs->w
    };

    return result;
}

internal vec2_t vec2_mul_s(vec2_t* lhs, f32 rhs)
{
    vec2_t result = {
        lhs->x * rhs,
        lhs->y * rhs
    };

    return result;
}

internal vec3_t vec3_mul_s(vec3_t* lhs, f32 rhs)
{
    vec3_t result = {
        lhs->x * rhs,
        lhs->y * rhs,
        lhs->z * rhs
    };

    return result;
}

internal vec4_t vec4_mul_s(vec4_t* lhs, f32 rhs)
{
    vec4_t result = {
        lhs->x * rhs,
        lhs->y * rhs,
        lhs->z * rhs,
        lhs->w * rhs
    };

    return result;
}

internal f32 vec2_dot(vec2_t* lhs, vec2_t* rhs)
{
    f32 result =
        lhs->x * rhs->x +
        lhs->y * rhs->y;

    return result;
}

internal f32 vec3_dot(vec3_t* lhs, vec3_t* rhs)
{
    f32 result =
        lhs->x * rhs->x +
        lhs->y * rhs->y +
        lhs->z * rhs->z;

    return result;
}

internal f32 vec4_dot(vec4_t* lhs, vec4_t* rhs)
{
    f32 result =
        lhs->x * rhs->x +
        lhs->y * rhs->y +
        lhs->z * rhs->z +
        lhs->w * rhs->w;

    return result;
}

internal f32 vec2_len(vec2_t* vector)
{
    f32 x_sqr = vector->x * vector->x;
    f32 y_sqr = vector->y * vector->y;

    f32 result = math_sqrt(x_sqr + y_sqr);

    return result;
}

internal f32 vec3_len(vec3_t* vector)
{
    f32 x_sqr = vector->x * vector->x;
    f32 y_sqr = vector->y * vector->y;
    f32 z_sqr = vector->z * vector->z;

    f32 result = math_sqrt(x_sqr + y_sqr + z_sqr);

    return result;
}

internal f32 vec4_len(vec4_t* vector)
{
    f32 x_sqr = vector->x * vector->x;
    f32 y_sqr = vector->y * vector->y;
    f32 z_sqr = vector->z * vector->z;
    f32 w_sqr = vector->w * vector->w;

    f32 result = math_sqrt(x_sqr + y_sqr + z_sqr + w_sqr);

    return result;
}

internal f32 vec2_len_sqr(vec2_t* vector)
{
    f32 result =
        vector->x * vector->x +
        vector->y * vector->y;

    return result;
}

internal f32 vec3_len_sqr(vec3_t* vector)
{
    f32 result =
        vector->x * vector->x +
        vector->y * vector->y +
        vector->z * vector->z;

    return result;
}

internal f32 vec4_len_sqr(vec4_t* vector)
{
    f32 result =
        vector->x * vector->x +
        vector->y * vector->y +
        vector->z * vector->z +
        vector->w * vector->w;

    return result;
}

internal vec3_t vec3_cross(vec3_t* lhs, vec3_t* rhs)
{
    vec3_t result = {
        lhs->y * rhs->z - lhs->z * rhs->y,
        lhs->z * rhs->x - lhs->x * rhs->z,
        lhs->x * rhs->y - lhs->y * rhs->x
    };

    return result;
}

internal vec3_t vec3_rotate_quat(vec3_t* vector, quat_t* q)
{
    quat_t vec_q = {vector->x, vector->y, vector->z, 0.0f};
    quat_t conj  = quat_conj(q);
    quat_t res_q = quat_mul(q, &vec_q);
    res_q        = quat_mul(&res_q, &conj);

    vec3_t result = {res_q.x, res_q.y, res_q.z};

    return result;
}

internal vec3_t vec3_rotate_axis(vec3_t* vector, vec3_t* axis, f32 angle)
{
    quat_t rot    = quat_from_axis_angle(axis, angle);
    vec3_t result = vec3_rotate_quat(vector, &rot);

    return result;
}

