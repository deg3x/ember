internal quat_t quat_norm(quat_t* q)
{
    f32 inv_len = 1.0f / quat_len(q);

    quat_t result = {
        q->x * inv_len,
        q->y * inv_len,
        q->z * inv_len,
        q->w * inv_len
    };

    return result;
}

internal quat_t quat_renorm(quat_t* q)
{
    f32 inv_len = math_fast_inv_sqrt_approx_one(quat_len_sqr(q));

    quat_t result = {
        q->x * inv_len,
        q->y * inv_len,
        q->z * inv_len,
        q->w * inv_len
    };

    return result;
}

internal quat_t quat_add(quat_t* lhs, quat_t* rhs)
{
    quat_t result = {
        lhs->x + rhs->x,
        lhs->y + rhs->y,
        lhs->z + rhs->z,
        lhs->w + rhs->w
    };

    return result;
}

internal quat_t quat_sub(quat_t* lhs, quat_t* rhs)
{
    quat_t result = {
        lhs->x - rhs->x,
        lhs->y - rhs->y,
        lhs->z - rhs->z,
        lhs->w - rhs->w
    };

    return result;
}

internal quat_t quat_mul(quat_t* lhs, quat_t* rhs)
{
    f32 x = lhs->w * rhs->x + lhs->x * rhs->w + lhs->y * rhs->z - lhs->z * rhs->y;
    f32 y = lhs->w * rhs->y + lhs->y * rhs->w - lhs->x * rhs->z + lhs->z * rhs->x;
    f32 z = lhs->w * rhs->z + lhs->z * rhs->w + lhs->x * rhs->y - lhs->y * rhs->x;
    f32 w = lhs->w * rhs->w - lhs->x * rhs->x - lhs->y * rhs->y - lhs->z * rhs->z;

    quat_t result = { x, y, z, w };

    return result;
}

internal quat_t quat_mul_s(quat_t* lhs, f32 rhs)
{
    quat_t result = {
        lhs->x * rhs,
        lhs->y * rhs,
        lhs->z * rhs,
        lhs->w * rhs
    };

    return result;
}

internal quat_t quat_inv(quat_t* q)
{
    f32 inv_len_sqr = 1.0f / quat_len_sqr(q);

    quat_t result = {
       -q->x * inv_len_sqr,
       -q->y * inv_len_sqr,
       -q->z * inv_len_sqr,
        q->w * inv_len_sqr
    };

    return result;
}

internal quat_t quat_conj(quat_t* q)
{
    quat_t result = {
       -q->x,
       -q->y,
       -q->z,
        q->w
    };

    return result;
}

internal f32 quat_dot(quat_t* lhs, quat_t* rhs)
{
    f32 result =
        lhs->x * rhs->x +
        lhs->y * rhs->y +
        lhs->z * rhs->z +
        lhs->w * rhs->w;

    return result;
}

internal f32 quat_len(quat_t* q)
{
    f32 x_sqr = q->x * q->x;
    f32 y_sqr = q->y * q->y;
    f32 z_sqr = q->z * q->z;
    f32 w_sqr = q->w * q->w;

    f32 result = math_sqrt(x_sqr + y_sqr + z_sqr + w_sqr);

    return result;
}

internal f32 quat_len_sqr(quat_t* q)
{
    f32 result =
        q->x * q->x +
        q->y * q->y +
        q->z * q->z +
        q->w * q->w;

    return result;
}
