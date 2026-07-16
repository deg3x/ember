internal b32 math_approx_zero(f32 value)
{
    return (value > -MATH_FLT_MIN) && (value < MATH_FLT_MIN);
}

internal b32 math_approx_equal(f32 a, f32 b, f32 error)
{
    return ((a + error) > b) && ((a - error) < b);
}

internal f32 math_sign(f32 value)
{
    if (value < 0.0f)
    {
        return -1.0f;
    }

    if (value > 0.0f)
    {
        return 1.0f;
    }

    return 0.0f;
}

internal f32 math_angle_wrap(f32 angle)
{
    f32 result = math_mod(angle, MATH_TWO_PI);

    if (result < 0.0f)
    {
        return result + MATH_TWO_PI;
    }

    return result;
}

internal f32 math_saturate(f32 value)
{
    return CLAMP(value, 0.0f, 1.0f);
}

internal f32 math_fast_sin(f32 angle)
{
    return math_fast_cos(angle - MATH_HALF_PI);
}

internal f32 math_fast_cos(f32 angle)
{
    f32 angle_wrapped = math_angle_wrap(angle);

    if (angle_wrapped < MATH_HALF_PI)
    {
        return math_fast_cos_zero_to_half_pi(angle_wrapped);
    }

    if (angle_wrapped < MATH_PI)
    {
        return -math_fast_cos_zero_to_half_pi(MATH_PI - angle_wrapped);
    }

    if (angle_wrapped < MATH_THREE_PI_HALF)
    {
        return -math_fast_cos_zero_to_half_pi(angle_wrapped - MATH_PI);
    }

    return math_fast_cos_zero_to_half_pi(MATH_TWO_PI - angle_wrapped);
}

internal f32 math_fast_cos_zero_to_half_pi(f32 angle)
{
    f32 a0 = 1.0f;
    f32 a2 = 2.0f * MATH_PI_INV - 12.0f * MATH_PI_SQR_INV;
    f32 a3 = 16.0f * MATH_PI_CUBE_INV - 4.0f * MATH_PI_SQR_INV;

    f32 angle_sqr = angle * angle;

    return a0 + a2 * angle_sqr + a3 * angle_sqr * angle;
}

internal f32 math_fast_inv_sqrt_approx_one(f32 value)
{
    f32 a0 =  1.875f;
    f32 a1 = -1.250f;
    f32 a2 =  0.375f;

    return a0 + a1 * value + a2 * value * value;
}

internal vec3_t vec3_rotate_quat(vec3_t* vector, quat_t* quat)
{
    quat_t vec_q = {vector->x, vector->y, vector->z, 0.0f};
    quat_t conj  = quat_conj(quat);
    quat_t res_q = quat_mul(quat, &vec_q);
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

internal vec3_t euler_from_quat(quat_t* quat)
{
    vec3_t result;
    f32 pitch_test = quat->w * quat->x + quat->y * quat->z;
    f32 pitch_sign = math_sign(pitch_test);

    if (math_approx_equal(math_abs(pitch_test), 0.5f, MATH_EPSILON))
    {
        result.x = MATH_HALF_PI;
        result.y = pitch_sign * math_atan2(quat->y, quat->x);
        result.z = 0.0f;

        return result;
    }

    f32 xx = quat->x * quat->x;
    f32 yy = quat->y * quat->y;
    f32 zz = quat->z * quat->z;
    f32 xy = quat->x * quat->y;
    f32 xz = quat->x * quat->z;
    f32 wy = quat->w * quat->y;
    f32 wz = quat->w * quat->z;

    f32 pitch = math_atan2(2.0f * pitch_test, 1.0f - 2.0f * (xx + yy));
    f32 yaw   = -math_asin(2.0f * (xz - wy));
    f32 roll  = math_atan2(2.0f * (wz + xy), 1.0f - 2.0f * (yy + zz));

    result.x = pitch;
    result.y = yaw;
    result.z = roll;

    return result;
}

internal vec3_t euler_from_mat4(mat4_t* matrix)
{
    f32 pitch =  math_atan2(matrix->m[1][2], matrix->m[2][2]);
    f32 roll  =  math_atan2(matrix->m[0][1], matrix->m[0][0]);
    f32 yaw   = -math_asin(matrix->m[0][2]);

    vec3_t result = { pitch, yaw, roll };

    return result;
}

internal mat4_t mat4_from_euler(vec3_t* angles)
{
    f32 cos_x = math_cos(angles->x);
    f32 cos_y = math_cos(angles->y);
    f32 cos_z = math_cos(angles->z);
    f32 sin_x = math_sin(angles->x);
    f32 sin_y = math_sin(angles->y);
    f32 sin_z = math_sin(angles->z);

    mat4_t result = {0};

    result.m[0][0] =  cos_y * cos_z;
    result.m[0][1] =  sin_z * cos_y;
    result.m[0][2] = -sin_y;

    result.m[1][0] =  sin_y * sin_x * cos_z - cos_x * sin_z;
    result.m[1][1] =  sin_y * sin_x * sin_z + cos_x * cos_z;
    result.m[1][2] =  cos_y * sin_x;

    result.m[2][0] =  sin_y * cos_x * cos_z + sin_x * sin_z;
    result.m[2][1] =  sin_y * cos_x * sin_z - sin_x * cos_z;
    result.m[2][2] =  cos_y * cos_x;

    result.m[3][3] =  1.0f;

    return result;
}

internal mat4_t mat4_from_pitch(f32 pitch)
{
    mat4_t result = {0};

    f32 angle_cos = math_cos(pitch);
    f32 angle_sin = math_sin(pitch);

    result.m[1][1] =  angle_cos;
    result.m[1][2] =  angle_sin;
    result.m[2][1] = -angle_sin;
    result.m[2][2] =  angle_cos;
    result.m[0][0] =  1.0f;
    result.m[3][3] =  1.0f;

    return result;
}

internal mat4_t mat4_from_yaw(f32 yaw)
{
    mat4_t result = {0};

    f32 angle_cos = math_cos(yaw);
    f32 angle_sin = math_sin(yaw);

    result.m[0][0] =  angle_cos;
    result.m[0][2] = -angle_sin;
    result.m[2][0] =  angle_sin;
    result.m[2][2] =  angle_cos;
    result.m[1][1] =  1.0f;
    result.m[3][3] =  1.0f;

    return result;
}

internal mat4_t mat4_from_roll(f32 roll)
{
    mat4_t result = {0};

    f32 angle_cos = math_cos(roll);
    f32 angle_sin = math_sin(roll);

    result.m[0][0] =  angle_cos;
    result.m[0][1] =  angle_sin;
    result.m[1][0] = -angle_sin;
    result.m[1][1] =  angle_cos;
    result.m[2][2] =  1.0f;
    result.m[3][3] =  1.0f;

    return result;
}

internal mat4_t mat4_from_diag(f32 diagonal)
{
    mat4_t result = {
        .m = {
            { diagonal, 0.0f, 0.0f, 0.0f },
            { 0.0f, diagonal, 0.0f, 0.0f },
            { 0.0f, 0.0f, diagonal, 0.0f },
            { 0.0f, 0.0f, 0.0f, diagonal }
        }
    };

    return result;
}

internal mat4_t mat4_from_quat(quat_t* quat)
{
    mat4_t result;

    f32 ww = quat->w * quat->w;
    f32 wx = quat->w * quat->x;
    f32 wy = quat->w * quat->y;
    f32 wz = quat->w * quat->z;
    f32 xx = quat->x * quat->x;
    f32 xy = quat->x * quat->y;
    f32 xz = quat->x * quat->z;
    f32 yy = quat->y * quat->y;
    f32 yz = quat->y * quat->z;
    f32 zz = quat->z * quat->z;

    result.m[0][0] = 2.0f * (ww + xx) - 1.0f;
    result.m[0][1] = 2.0f * (xy + wz);
    result.m[0][2] = 2.0f * (xz - wy);
    result.m[0][3] = 0.0f;
    result.m[1][0] = 2.0f * (xy - wz);
    result.m[1][1] = 2.0f * (ww + yy) - 1.0f;
    result.m[1][2] = 2.0f * (wx + yz);
    result.m[1][3] = 0.0f;
    result.m[2][0] = 2.0f * (wy + xz);
    result.m[2][1] = 2.0f * (yz - wx);
    result.m[2][2] = 2.0f * (ww + zz) - 1.0f;
    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}

internal mat4_t mat4_translation(vec3_t* translation)
{
    mat4_t result = {
        .m = {
            { 1.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f, 0.0f },
            { translation->x, translation->y, translation->z, 1.0f }
        }
    };

    return result;
}

internal mat4_t mat4_rotation(vec3_t* axis, f32 angle)
{
    f32 angle_cos   = math_cos(angle);
    f32 angle_sin   = math_sin(angle);
    f32 angle_sin_x = axis->x * angle_sin;
    f32 angle_sin_y = axis->y * angle_sin;
    f32 angle_sin_z = axis->z * angle_sin;
    f32 one_sub_cos = 1.0f - angle_cos;

    mat4_t result;

    result.m[0][0] = axis->x * axis->x * one_sub_cos + angle_cos;
    result.m[0][1] = axis->x * axis->y * one_sub_cos + angle_sin_z;
    result.m[0][2] = axis->x * axis->z * one_sub_cos - angle_sin_y;
    result.m[0][3] = 0.0f;

    result.m[1][0] = axis->x * axis->y * one_sub_cos - angle_sin_z;
    result.m[1][1] = axis->y * axis->y * one_sub_cos + angle_cos;
    result.m[1][2] = axis->y * axis->z * one_sub_cos + angle_sin_x;
    result.m[1][3] = 0.0f;

    result.m[2][0] = axis->x * axis->z * one_sub_cos + angle_sin_y;
    result.m[2][1] = axis->y * axis->z * one_sub_cos - angle_sin_x;
    result.m[2][2] = axis->z * axis->z * one_sub_cos + angle_cos;
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}

internal mat4_t mat4_scale(vec3_t* scale)
{
    mat4_t result = {
        .m = {
            { scale->x, 0.0f, 0.0f, 0.0f },
            { 0.0f, scale->y, 0.0f, 0.0f },
            { 0.0f, 0.0f, scale->z, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        }
    };

    return result;
}

internal mat4_t mat4_model(vec3_t* position, quat_t* rotation, vec3_t* scale)
{
    mat4_t rot_matrix = mat4_from_quat(rotation);
    mat4_t scl_matrix = mat4_scale(scale);

    mat4_t result = mat4_translation(position);
    result        = mat4_mul(&result, &rot_matrix);
    result        = mat4_mul(&result, &scl_matrix);

    return result;
}

internal mat4_t mat4_persp(f32 fov, f32 aspect_ratio, f32 clip_near, f32 clip_far)
{
    f32 fov_rad              = fov * MATH_DEG2RAD;
    f32 inv_half_fov_tan     = 1.0f / math_tan(fov_rad * 0.5f);
    f32 inv_aspect_ratio     = 1.0f / aspect_ratio;
    f32 far_div_far_min_near = clip_far / (clip_far - clip_near);

    mat4_t result = {0};

    result.m[0][0] = inv_half_fov_tan * inv_aspect_ratio;
    result.m[1][1] = inv_half_fov_tan;
    result.m[2][2] = far_div_far_min_near;
    result.m[2][3] = 1.0f;
    result.m[3][2] = -clip_near * far_div_far_min_near;

    return result;
}

internal mat4_t mat4_ortho(f32 clip_left, f32 clip_right, f32 clip_bottom, f32 clip_top, f32 clip_near, f32 clip_far)
{
    mat4_t result = {0};

    f32 inv_right_minus_left = 1.0f / (clip_right - clip_left);
    f32 inv_top_minus_bottom = 1.0f / (clip_top - clip_bottom);
    f32 inv_far_minus_near   = 1.0f / (clip_far - clip_near);

    result.m[0][0] = 2.0f * inv_right_minus_left;
    result.m[1][1] = 2.0f * inv_top_minus_bottom;
    result.m[2][2] = 2.0f * inv_far_minus_near;
    result.m[3][0] = -(clip_right + clip_left) * inv_right_minus_left;
    result.m[3][1] = -(clip_top + clip_bottom) * inv_top_minus_bottom;
    result.m[3][2] = -(clip_far + clip_near) * inv_far_minus_near;
    result.m[3][3] = 1.0f;

    return result;
}

internal mat4_t mat4_look_at(vec3_t* eye, vec3_t* target, vec3_t* up)
{
    vec3_t eye_to_target = vec3_sub(target, eye);
    vec3_t forward       = vec3_norm(&eye_to_target);
    vec3_t up_cross_fw   = vec3_cross(up, &forward);
    vec3_t right         = vec3_norm(&up_cross_fw);
    vec3_t fw_cross_r    = vec3_cross(&forward, &right);
    vec3_t local_up      = vec3_norm(&fw_cross_r);

    mat4_t result;

    result.m[0][0] = right.x;
    result.m[0][1] = local_up.x;
    result.m[0][2] = forward.x;
    result.m[0][3] = 0.0f;

    result.m[1][0] = right.y;
    result.m[1][1] = local_up.y;
    result.m[1][2] = forward.y;
    result.m[1][3] = 0.0f;

    result.m[2][0] = right.z;
    result.m[2][1] = local_up.z;
    result.m[2][2] = forward.z;
    result.m[2][3] = 0.0f;

    result.m[3][0] = -vec3_dot(&right, eye);
    result.m[3][1] = -vec3_dot(&local_up, eye);
    result.m[3][2] = -vec3_dot(&forward, eye);
    result.m[3][3] = 1.0f;

    return result;
}
