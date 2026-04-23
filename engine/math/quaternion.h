#ifndef QUATERNION_H
#define QUATERNION_H

internal quat_t quat_norm(quat_t* q);
internal quat_t quat_renorm(quat_t* q);

internal quat_t quat_add(quat_t* lhs, quat_t* rhs);
internal quat_t quat_sub(quat_t* lhs, quat_t* rhs);
internal quat_t quat_mul(quat_t* lhs, quat_t* rhs);
internal quat_t quat_mul_s(quat_t* lhs, f32 rhs);

internal quat_t quat_inv(quat_t* q);
internal quat_t quat_conj(quat_t* q);

internal f32 quat_dot(quat_t* lhs, quat_t* rhs);
internal f32 quat_len(quat_t* q);
internal f32 quat_len_sqr(quat_t* q);

internal vec3_t quat_to_euler(quat_t* q);
internal quat_t quat_from_euler(vec3_t* angles);
internal quat_t quat_from_matrix(mat4_t* matrix);
internal quat_t quat_from_axis_angle(vec3_t* axis, f32 angle);

#endif
