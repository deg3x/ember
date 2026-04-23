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

internal vec3_t quat_to_euler(quat_t* q)
{
    vec3_t result;
    f32 pitch_test = q->w * q->x + q->y * q->z;
    f32 pitch_sign = math_sign(pitch_test);

    if (math_approx_equal(math_abs(pitch_test), 0.5f, MATH_EPSILON))
    {
        result.x = MATH_HALF_PI;
        result.y = pitch_sign * math_atan2(q->y, q->x);
        result.z = 0.0f;

        return result;
    }

    f32 xx = q->x * q->x;
    f32 yy = q->y * q->y;
    f32 zz = q->z * q->z;
    f32 xy = q->x * q->y;
    f32 xz = q->x * q->z;
    f32 wy = q->w * q->y;
    f32 wz = q->w * q->z;

    f32 pitch = math_atan2(2.0f * pitch_test, 1.0f - 2.0f * (xx + yy));
    f32 yaw   = -math_asin(2.0f * (xz - wy));
    f32 roll  = math_atan2(2.0f * (wz + xy), 1.0f - 2.0f * (yy + zz));

    result.x = pitch;
    result.y = yaw;
    result.z = roll;

    return result;
}

internal quat_t quat_from_euler(vec3_t* angles)
{
    f32 x_half = angles->x;
    f32 y_half = angles->y;
    f32 z_half = angles->z;
    f32 cos_p  = math_cos(x_half);
    f32 cos_y  = math_cos(y_half);
    f32 cos_r  = math_cos(z_half);
    f32 sin_p  = math_sin(x_half);
    f32 sin_y  = math_sin(y_half);
    f32 sin_r  = math_sin(z_half);

    f32 x = sin_p * cos_y * cos_r - cos_p * sin_y * sin_r;
    f32 y = cos_p * sin_y * cos_r + sin_p * cos_y * sin_r;
    f32 z = cos_p * cos_y * sin_r - sin_p * sin_y * cos_r;
    f32 w = cos_p * cos_y * cos_r + sin_p * sin_y * sin_r;

    quat_t result = { x, y, z, w };

    return result;
}

internal quat_t quat_from_matrix(mat4_t* matrix)
{
    f32 m00m11_a = matrix->m[0][0] + matrix->m[1][1];
    f32 m00m11_s = matrix->m[0][0] - matrix->m[1][1];
    f32 m22m33_a = matrix->m[2][2] + matrix->m[3][3];

    f32 four_x_sqr = m00m11_s - m22m33_a;
    f32 four_y_sqr = m00m11_a - m22m33_a;
    f32 four_z_sqr = m00m11_s + m22m33_a;
    f32 four_w_sqr = m00m11_a + m22m33_a;

    f32 max_comp = four_w_sqr;
    u32 max_idx  = 3;

    if (four_x_sqr > max_comp)
    {
        max_comp = four_x_sqr;
        max_idx  = 0;
    }
    if (four_y_sqr > max_comp)
    {
        max_comp = four_y_sqr;
        max_idx  = 1;
    }
    if (four_z_sqr > max_comp)
    {
        max_comp = four_z_sqr;
        max_idx  = 2;
    }

    f32 ops_sign[4][3] = {
        {-1.0f,  1.0f,  1.0f},
        { 1.0f, -1.0f,  1.0f},
        { 1.0f,  1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
    };

    f32 max_sqrt     = math_sqrt(max_comp);
    f32 inv_two_sqrt = 1.0f / (2.0f * max_sqrt);

    quat_t result;
    result.data[max_idx]           = 0.5f * max_sqrt;
    result.data[(max_idx + 1) % 4] = (matrix->m[1][2] + ops_sign[max_idx][0] * matrix->m[2][1]) * inv_two_sqrt;
    result.data[(max_idx + 2) % 4] = (matrix->m[2][0] + ops_sign[max_idx][1] * matrix->m[0][2]) * inv_two_sqrt;
    result.data[(max_idx + 3) % 4] = (matrix->m[0][1] + ops_sign[max_idx][2] * matrix->m[1][0]) * inv_two_sqrt;

    return quat_norm(&result);
}

internal quat_t quat_from_axis_angle(vec3_t* axis, f32 angle)
{
    f32 half_angle     = angle * 0.5f;
    f32 half_angle_cos = math_cos(half_angle);
    f32 half_angle_sin = math_sin(half_angle);

    quat_t result = {
        half_angle_sin * axis->x,
        half_angle_sin * axis->y,
        half_angle_sin * axis->z,
        half_angle_cos
    };

    return result;
}

