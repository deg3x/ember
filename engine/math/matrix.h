#ifndef MATRIX_H
#define MATRIX_H

f32 mat4_determinant(mat4* matrix);

mat4 mat4_add(mat4* lhs, mat4* rhs);
mat4 mat4_sub(mat4* lhs, mat4* rhs);
mat4 mat4_mul(mat4* lhs, mat4* rhs);
mat4 mat4_mul_s(mat4* lhs, f32 rhs);

mat4 mat4_inverse(mat4* matrix);
mat4 mat4_transpose(mat4* matrix);

vec3 mat4_to_euler(mat4* matrix);
mat4 mat4_from_euler(vec3* angles);
mat4 mat4_from_pitch(f32 pitch);
mat4 mat4_from_yaw(f32 yaw);
mat4 mat4_from_roll(f32 roll);
mat4 mat4_from_diag(f32 diagonal);
mat4 mat4_from_quat(quat* quat);

mat4 mat4_translation(vec3* translation);
mat4 mat4_rotation(vec3* axis, f32 angle);
mat4 mat4_scale(vec3* scale);

mat4 mat4_model(vec3* position, quat* rotation, vec3* scale);
mat4 mat4_persp(f32 fov_x, f32 aspect_ratio, f32 clip_near, f32 clip_far);
mat4 mat4_ortho(f32 clip_left, f32 clip_right, f32 clip_bottom, f32 clip_top, f32 clip_near, f32 clip_far);
mat4 mat4_look_at(vec3* eye, vec3* target, vec3* up);

#endif // MATRIX_H
