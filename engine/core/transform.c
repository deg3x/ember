internal transform_t transform_from_mat4(mat4_t* matrix)
{
    // NOTE(KB): This function will break when there's a negative scale or shear in the matrix
    //           Fix when it becomes an issue

    transform_t result;

    f32 scale_x = vec3_len(&(vec3_t){matrix->cols[0].x, matrix->cols[0].y, matrix->cols[0].z});
    f32 scale_y = vec3_len(&(vec3_t){matrix->cols[1].x, matrix->cols[1].y, matrix->cols[1].z});
    f32 scale_z = vec3_len(&(vec3_t){matrix->cols[2].x, matrix->cols[2].y, matrix->cols[2].z});

    f32 inv_scale_x = 1.0f / scale_x;
    f32 inv_scale_y = 1.0f / scale_y;
    f32 inv_scale_z = 1.0f / scale_z;

    vec4_t rot_a = vec4_mul_s(&matrix->cols[0], inv_scale_x);
    vec4_t rot_b = vec4_mul_s(&matrix->cols[1], inv_scale_y);
    vec4_t rot_c = vec4_mul_s(&matrix->cols[2], inv_scale_z);
    vec4_t rot_d = (vec4_t){0.0f, 0.0f, 0.0f, 1.0f};

    mat4_t rot_m = {rot_a, rot_b, rot_c, rot_d};

    result.position = (vec3_t){matrix->cols[3].x, matrix->cols[3].y, matrix->cols[3].z};
    result.rotation = quat_from_matrix(&rot_m);
    result.scale    = (vec3_t){scale_x, scale_y, scale_z};

    return result;
}

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
