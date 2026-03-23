vec2 vec2_norm(vec2* vector)
{
    f32 inv_len = 1.0f / vec2_len(vector);

    vec2 result = {
        vector->x * inv_len,
        vector->y * inv_len
    };

    return result;
}

vec3 vec3_norm(vec3* vector)
{
    f32 inv_len = 1.0f / vec3_len(vector);

    vec3 result = {
        vector->x * inv_len,
        vector->y * inv_len,
        vector->z * inv_len
    };

    return result;
}

vec4 vec4_norm(vec4* vector)
{
    f32 inv_len = 1.0f / vec4_len(vector);

    vec4 result = {
        vector->x * inv_len,
        vector->y * inv_len,
        vector->z * inv_len,
        vector->w * inv_len
    };

    return result;
}

vec2 vec2_neg(vec2* vector)
{
    vec2 result = {
        -vector->x,
        -vector->y
    };

    return result;
}

vec3 vec3_neg(vec3* vector)
{
    vec3 result = {
        -vector->x,
        -vector->y,
        -vector->z
    };

    return result;
}

vec4 vec4_neg(vec4* vector)
{
    vec4 result = {
        -vector->x,
        -vector->y,
        -vector->z,
        -vector->w
    };

    return result;
}

vec2 vec2_add(vec2* lhs, vec2* rhs)
{
    vec2 result = {
        lhs->x + rhs->x,
        lhs->y + rhs->y
    };

    return result;
}

vec3 vec3_add(vec3* lhs, vec3* rhs)
{
    vec3 result = {
        lhs->x + rhs->x,
        lhs->y + rhs->y,
        lhs->z + rhs->z
    };

    return result;
}

vec4 vec4_add(vec4* lhs, vec4* rhs)
{
    vec4 result = {
        lhs->x + rhs->x,
        lhs->y + rhs->y,
        lhs->z + rhs->z,
        lhs->w + rhs->w
    };

    return result;
}

vec2 vec2_sub(vec2* lhs, vec2* rhs)
{
    vec2 result = {
        lhs->x - rhs->x,
        lhs->y - rhs->y
    };

    return result;
}

vec3 vec3_sub(vec3* lhs, vec3* rhs)
{
    vec3 result = {
        lhs->x - rhs->x,
        lhs->y - rhs->y,
        lhs->z - rhs->z
    };

    return result;
}

vec4 vec4_sub(vec4* lhs, vec4* rhs)
{
    vec4 result = {
        lhs->x - rhs->x,
        lhs->y - rhs->y,
        lhs->z - rhs->z,
        lhs->w - rhs->w
    };

    return result;
}

vec2 vec2_mul(vec2* lhs, vec2* rhs)
{
    vec2 result = {
        lhs->x * rhs->x,
        lhs->y * rhs->y
    };

    return result;
}

vec3 vec3_mul(vec3* lhs, vec3* rhs)
{
    vec3 result = {
        lhs->x * rhs->x,
        lhs->y * rhs->y,
        lhs->z * rhs->z
    };

    return result;
}

vec4 vec4_mul(vec4* lhs, vec4* rhs)
{
    vec4 result = {
        lhs->x * rhs->x,
        lhs->y * rhs->y,
        lhs->z * rhs->z,
        lhs->w * rhs->w
    };

    return result;
}

vec2 vec2_mul_s(vec2* lhs, f32 rhs)
{
    vec2 result = {
        lhs->x * rhs,
        lhs->y * rhs
    };

    return result;
}

vec3 vec3_mul_s(vec3* lhs, f32 rhs)
{
    vec3 result = {
        lhs->x * rhs,
        lhs->y * rhs,
        lhs->z * rhs
    };

    return result;
}

vec4 vec4_mul_s(vec4* lhs, f32 rhs)
{
    vec4 result = {
        lhs->x * rhs,
        lhs->y * rhs,
        lhs->z * rhs,
        lhs->w * rhs
    };

    return result;
}

f32 vec2_dot(vec2* lhs, vec2* rhs)
{
    f32 result =
        lhs->x * rhs->x +
        lhs->y * rhs->y;

    return result;
}

f32 vec3_dot(vec3* lhs, vec3* rhs)
{
    f32 result =
        lhs->x * rhs->x +
        lhs->y * rhs->y +
        lhs->z * rhs->z;

    return result;
}

f32 vec4_dot(vec4* lhs, vec4* rhs)
{
    f32 result =
        lhs->x * rhs->x +
        lhs->y * rhs->y +
        lhs->z * rhs->z +
        lhs->w * rhs->w;

    return result;
}

f32 vec2_len(vec2* vector)
{
    f32 x_sqr = vector->x * vector->x;
    f32 y_sqr = vector->y * vector->y;

    f32 result = math_sqrt(x_sqr + y_sqr);

    return result;
}

f32 vec3_len(vec3* vector)
{
    f32 x_sqr = vector->x * vector->x;
    f32 y_sqr = vector->y * vector->y;
    f32 z_sqr = vector->z * vector->z;

    f32 result = math_sqrt(x_sqr + y_sqr + z_sqr);

    return result;
}

f32 vec4_len(vec4* vector)
{
    f32 x_sqr = vector->x * vector->x;
    f32 y_sqr = vector->y * vector->y;
    f32 z_sqr = vector->z * vector->z;
    f32 w_sqr = vector->w * vector->w;

    f32 result = math_sqrt(x_sqr + y_sqr + z_sqr + w_sqr);

    return result;
}

f32 vec2_len_sqr(vec2* vector)
{
    f32 result =
        vector->x * vector->x +
        vector->y * vector->y;

    return result;
}

f32 vec3_len_sqr(vec3* vector)
{
    f32 result =
        vector->x * vector->x +
        vector->y * vector->y +
        vector->z * vector->z;

    return result;
}

f32 vec4_len_sqr(vec4* vector)
{
    f32 result =
        vector->x * vector->x +
        vector->y * vector->y +
        vector->z * vector->z +
        vector->w * vector->w;

    return result;
}

vec3 vec3_cross(vec3* lhs, vec3* rhs)
{
    vec3 result = {
        lhs->y * rhs->z - lhs->z * rhs->y,
        lhs->z * rhs->x - lhs->x * rhs->z,
        lhs->x * rhs->y - lhs->y * rhs->x
    };

    return result;
}

vec3 vec3_rotate_quat(vec3* vector, quat* q)
{
    quat vec_q = {vector->x, vector->y, vector->z, 1.0f};
    quat conj  = quat_conj(q);
    quat res_q = quat_mul(q, &vec_q);
    res_q      = quat_mul(&res_q, &conj);

    vec3 result = {res_q.x, res_q.y, res_q.z};

    return result;
}

vec3 vec3_rotate_axis(vec3* vector, vec3* axis, f32 angle)
{
    quat rot    = quat_from_axis_angle(axis, angle);
    vec3 result = vec3_rotate_quat(vector, &rot);

    return result;
}

