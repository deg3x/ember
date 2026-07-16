internal f32 mat4_determinant(mat4_t* matrix)
{
    f32 det_23 = matrix->m[2][2] * matrix->m[3][3] - matrix->m[2][3] * matrix->m[3][2];
    f32 det_13 = matrix->m[1][2] * matrix->m[3][3] - matrix->m[1][3] * matrix->m[3][2];
    f32 det_12 = matrix->m[1][2] * matrix->m[2][3] - matrix->m[1][3] * matrix->m[2][2];
    f32 det_03 = matrix->m[0][2] * matrix->m[3][3] - matrix->m[0][3] * matrix->m[3][2];
    f32 det_02 = matrix->m[0][2] * matrix->m[2][3] - matrix->m[0][3] * matrix->m[2][2];
    f32 det_01 = matrix->m[0][2] * matrix->m[1][3] - matrix->m[0][3] * matrix->m[1][2];

    f32 det_123 = matrix->m[1][1] * det_23 - matrix->m[2][1] * det_13 + matrix->m[3][1] * det_12;
    f32 det_023 = matrix->m[0][1] * det_23 - matrix->m[2][1] * det_03 + matrix->m[3][1] * det_02;
    f32 det_013 = matrix->m[0][1] * det_13 - matrix->m[1][1] * det_03 + matrix->m[3][1] * det_01;
    f32 det_012 = matrix->m[0][1] * det_12 - matrix->m[1][1] * det_02 + matrix->m[2][1] * det_01;

    f32 result =
        matrix->m[0][0] * det_123 -
        matrix->m[1][0] * det_023 +
        matrix->m[2][0] * det_013 -
        matrix->m[3][0] * det_012;

    return result;
}

internal mat4_t mat4_add(mat4_t* lhs, mat4_t* rhs)
{
    mat4_t result = {{
        vec4_add(&lhs->cols[0], &rhs->cols[0]),
        vec4_add(&lhs->cols[1], &rhs->cols[1]),
        vec4_add(&lhs->cols[2], &rhs->cols[2]),
        vec4_add(&lhs->cols[3], &rhs->cols[3]),
    }};

    return result;
}

internal mat4_t mat4_sub(mat4_t* lhs, mat4_t* rhs)
{
    mat4_t result = {{
        vec4_sub(&lhs->cols[0], &rhs->cols[0]),
        vec4_sub(&lhs->cols[1], &rhs->cols[1]),
        vec4_sub(&lhs->cols[2], &rhs->cols[2]),
        vec4_sub(&lhs->cols[3], &rhs->cols[3]),
    }};

    return result;
}

internal mat4_t mat4_mul(mat4_t* lhs, mat4_t* rhs)
{
    mat4_t lhs_tr = mat4_transpose(lhs);

    mat4_t result = {
        .m = {
            {vec4_dot(&lhs_tr.cols[0], &rhs->cols[0]), vec4_dot(&lhs_tr.cols[1], &rhs->cols[0]), vec4_dot(&lhs_tr.cols[2], &rhs->cols[0]), vec4_dot(&lhs_tr.cols[3], &rhs->cols[0])},
            {vec4_dot(&lhs_tr.cols[0], &rhs->cols[1]), vec4_dot(&lhs_tr.cols[1], &rhs->cols[1]), vec4_dot(&lhs_tr.cols[2], &rhs->cols[1]), vec4_dot(&lhs_tr.cols[3], &rhs->cols[1])},
            {vec4_dot(&lhs_tr.cols[0], &rhs->cols[2]), vec4_dot(&lhs_tr.cols[1], &rhs->cols[2]), vec4_dot(&lhs_tr.cols[2], &rhs->cols[2]), vec4_dot(&lhs_tr.cols[3], &rhs->cols[2])},
            {vec4_dot(&lhs_tr.cols[0], &rhs->cols[3]), vec4_dot(&lhs_tr.cols[1], &rhs->cols[3]), vec4_dot(&lhs_tr.cols[2], &rhs->cols[3]), vec4_dot(&lhs_tr.cols[3], &rhs->cols[3])},
        }
    };

    return result;
}

internal mat4_t mat4_mul_s(mat4_t* lhs, f32 rhs)
{
    mat4_t result = {{
        vec4_mul_s(&lhs->cols[0], rhs),
        vec4_mul_s(&lhs->cols[1], rhs),
        vec4_mul_s(&lhs->cols[2], rhs),
        vec4_mul_s(&lhs->cols[3], rhs),
    }};

    return result;
}

internal mat4_t mat4_inverse(mat4_t* matrix)
{
    f32 det_23_23 = matrix->m[2][2] * matrix->m[3][3] - matrix->m[2][3] * matrix->m[3][2];
    f32 det_23_13 = matrix->m[1][2] * matrix->m[3][3] - matrix->m[1][3] * matrix->m[3][2];
    f32 det_23_12 = matrix->m[1][2] * matrix->m[2][3] - matrix->m[1][3] * matrix->m[2][2];
    f32 det_23_03 = matrix->m[0][2] * matrix->m[3][3] - matrix->m[0][3] * matrix->m[3][2];
    f32 det_23_02 = matrix->m[0][2] * matrix->m[2][3] - matrix->m[0][3] * matrix->m[2][2];
    f32 det_23_01 = matrix->m[0][2] * matrix->m[1][3] - matrix->m[0][3] * matrix->m[1][2];

    f32 det_123_123 = matrix->m[1][1] * det_23_23 - matrix->m[2][1] * det_23_13 + matrix->m[3][1] * det_23_12;
    f32 det_123_023 = matrix->m[0][1] * det_23_23 - matrix->m[2][1] * det_23_03 + matrix->m[3][1] * det_23_02;
    f32 det_123_013 = matrix->m[0][1] * det_23_13 - matrix->m[1][1] * det_23_03 + matrix->m[3][1] * det_23_01;
    f32 det_123_012 = matrix->m[0][1] * det_23_12 - matrix->m[1][1] * det_23_02 + matrix->m[2][1] * det_23_01;

    f32 determinant =
        matrix->m[0][0] * det_123_123 -
        matrix->m[1][0] * det_123_023 +
        matrix->m[2][0] * det_123_013 -
        matrix->m[3][0] * det_123_012;

    EMBER_ASSERT(!math_approx_zero(determinant));

    f32 det_13_23 = matrix->m[2][1] * matrix->m[3][3] - matrix->m[2][3] * matrix->m[3][1];
    f32 det_13_13 = matrix->m[1][1] * matrix->m[3][3] - matrix->m[1][3] * matrix->m[3][1];
    f32 det_13_12 = matrix->m[1][1] * matrix->m[2][3] - matrix->m[1][3] * matrix->m[2][1];
    f32 det_13_03 = matrix->m[0][1] * matrix->m[3][3] - matrix->m[0][3] * matrix->m[3][1];
    f32 det_13_02 = matrix->m[0][1] * matrix->m[2][3] - matrix->m[0][3] * matrix->m[2][1];
    f32 det_13_01 = matrix->m[0][1] * matrix->m[1][3] - matrix->m[0][3] * matrix->m[1][1];

    f32 det_12_23 = matrix->m[2][1] * matrix->m[3][2] - matrix->m[2][2] * matrix->m[3][1];
    f32 det_12_13 = matrix->m[1][1] * matrix->m[3][2] - matrix->m[1][2] * matrix->m[3][1];
    f32 det_12_12 = matrix->m[1][1] * matrix->m[2][2] - matrix->m[1][2] * matrix->m[2][1];
    f32 det_12_03 = matrix->m[0][1] * matrix->m[3][2] - matrix->m[0][2] * matrix->m[3][1];
    f32 det_12_02 = matrix->m[0][1] * matrix->m[2][2] - matrix->m[0][2] * matrix->m[2][1];
    f32 det_12_01 = matrix->m[0][1] * matrix->m[1][2] - matrix->m[0][2] * matrix->m[1][1];

    f32 det_023_123 = matrix->m[1][0] * det_23_23 - matrix->m[2][0] * det_23_13 + matrix->m[3][0] * det_23_12;
    f32 det_023_023 = matrix->m[0][0] * det_23_23 - matrix->m[2][0] * det_23_03 + matrix->m[3][0] * det_23_02;
    f32 det_023_013 = matrix->m[0][0] * det_23_13 - matrix->m[1][0] * det_23_03 + matrix->m[3][0] * det_23_01;
    f32 det_023_012 = matrix->m[0][0] * det_23_12 - matrix->m[1][0] * det_23_02 + matrix->m[2][0] * det_23_01;

    f32 det_013_123 = matrix->m[1][0] * det_13_23 - matrix->m[2][0] * det_13_13 + matrix->m[3][0] * det_13_12;
    f32 det_013_023 = matrix->m[0][0] * det_13_23 - matrix->m[2][0] * det_13_03 + matrix->m[3][0] * det_13_02;
    f32 det_013_013 = matrix->m[0][0] * det_13_13 - matrix->m[1][0] * det_13_03 + matrix->m[3][0] * det_13_01;
    f32 det_013_012 = matrix->m[0][0] * det_13_12 - matrix->m[1][0] * det_13_02 + matrix->m[2][0] * det_13_01;

    f32 det_012_123 = matrix->m[1][0] * det_12_23 - matrix->m[2][0] * det_12_13 + matrix->m[3][0] * det_12_12;
    f32 det_012_023 = matrix->m[0][0] * det_12_23 - matrix->m[2][0] * det_12_03 + matrix->m[3][0] * det_12_02;
    f32 det_012_013 = matrix->m[0][0] * det_12_13 - matrix->m[1][0] * det_12_03 + matrix->m[3][0] * det_12_01;
    f32 det_012_012 = matrix->m[0][0] * det_12_12 - matrix->m[1][0] * det_12_02 + matrix->m[2][0] * det_12_01;

    f32 inv_det = 1.0f / determinant;

    mat4_t result;

    result.m[0][0] =  det_123_123 * inv_det;
    result.m[0][1] = -det_123_023 * inv_det;
    result.m[0][2] =  det_123_013 * inv_det;
    result.m[0][3] = -det_123_012 * inv_det;

    result.m[1][0] = -det_023_123 * inv_det;
    result.m[1][1] =  det_023_023 * inv_det;
    result.m[1][2] = -det_023_013 * inv_det;
    result.m[1][3] =  det_023_012 * inv_det;

    result.m[2][0] =  det_013_123 * inv_det;
    result.m[2][1] = -det_013_023 * inv_det;
    result.m[2][2] =  det_013_013 * inv_det;
    result.m[2][3] = -det_013_012 * inv_det;

    result.m[3][0] = -det_012_123 * inv_det;
    result.m[3][1] =  det_012_023 * inv_det;
    result.m[3][2] = -det_012_013 * inv_det;
    result.m[3][3] =  det_012_012 * inv_det;

    return result;
}

internal mat4_t mat4_transpose(mat4_t* matrix)
{
    mat4_t result;

    result.m[0][0] = matrix->m[0][0];
    result.m[0][1] = matrix->m[1][0];
    result.m[0][2] = matrix->m[2][0];
    result.m[0][3] = matrix->m[3][0];

    result.m[1][0] = matrix->m[0][1];
    result.m[1][1] = matrix->m[1][1];
    result.m[1][2] = matrix->m[2][1];
    result.m[1][3] = matrix->m[3][1];

    result.m[2][0] = matrix->m[0][2];
    result.m[2][1] = matrix->m[1][2];
    result.m[2][2] = matrix->m[2][2];
    result.m[2][3] = matrix->m[3][2];

    result.m[3][0] = matrix->m[0][3];
    result.m[3][1] = matrix->m[1][3];
    result.m[3][2] = matrix->m[2][3];
    result.m[3][3] = matrix->m[3][3];

    return result;
}
