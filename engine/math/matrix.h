#ifndef MATRIX_H
#define MATRIX_H

internal f32 mat4_determinant(mat4_t* matrix);

internal mat4_t mat4_add(mat4_t* lhs, mat4_t* rhs);
internal mat4_t mat4_sub(mat4_t* lhs, mat4_t* rhs);
internal mat4_t mat4_mul(mat4_t* lhs, mat4_t* rhs);
internal mat4_t mat4_mul_s(mat4_t* lhs, f32 rhs);

internal mat4_t mat4_inverse(mat4_t* matrix);
internal mat4_t mat4_transpose(mat4_t* matrix);

internal vec3_t mat4_to_euler(mat4_t* matrix);
internal mat4_t mat4_from_euler(vec3_t* angles);
internal mat4_t mat4_from_pitch(f32 pitch);
internal mat4_t mat4_from_yaw(f32 yaw);
internal mat4_t mat4_from_roll(f32 roll);
internal mat4_t mat4_from_diag(f32 diagonal);
internal mat4_t mat4_from_quat(quat_t* quat);

internal mat4_t mat4_translation(vec3_t* translation);
internal mat4_t mat4_rotation(vec3_t* axis, f32 angle);
internal mat4_t mat4_scale(vec3_t* scale);

internal mat4_t mat4_model(vec3_t* position, quat_t* rotation, vec3_t* scale);
internal mat4_t mat4_persp(f32 fov_x, f32 aspect_ratio, f32 clip_near, f32 clip_far);
internal mat4_t mat4_ortho(f32 clip_left, f32 clip_right, f32 clip_bottom, f32 clip_top, f32 clip_near, f32 clip_far);
internal mat4_t mat4_look_at(vec3_t* eye, vec3_t* target, vec3_t* up);

#endif // MATRIX_H
