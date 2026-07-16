#ifndef MATH_CORE_H
#define MATH_CORE_H

internal b32 math_approx_zero(f32 value);
internal b32 math_approx_equal(f32 a, f32 b, f32 error);

internal f32 math_sign(f32 value);
internal f32 math_angle_wrap(f32 angle);
internal f32 math_saturate(f32 value);
internal f32 math_fast_sin(f32 angle);
internal f32 math_fast_cos(f32 angle);
internal f32 math_fast_cos_zero_to_half_pi(f32 angle);
internal f32 math_fast_inv_sqrt_approx_one(f32 value);

internal vec3_t vec3_rotate_quat(vec3_t* vector, quat_t* quat);
internal vec3_t vec3_rotate_axis(vec3_t* vector, vec3_t* axis, f32 angle);

internal quat_t quat_from_euler(vec3_t* angles);
internal quat_t quat_from_matrix(mat4_t* matrix);
internal quat_t quat_from_axis_angle(vec3_t* axis, f32 angle);

internal vec3_t euler_from_quat(quat_t* quat);
internal vec3_t euler_from_mat4(mat4_t* matrix);

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
internal mat4_t mat4_persp(f32 fov, f32 aspect_ratio, f32 clip_near, f32 clip_far);
internal mat4_t mat4_ortho(f32 clip_left, f32 clip_right, f32 clip_bottom, f32 clip_top, f32 clip_near, f32 clip_far);
internal mat4_t mat4_look_at(vec3_t* eye, vec3_t* target, vec3_t* up);

#endif // MATH_CORE_H
