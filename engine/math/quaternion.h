#ifndef QUATERNION_H
#define QUATERNION_H

internal quat quat_norm(quat* q);
internal quat quat_renorm(quat* q);

internal quat quat_add(quat* lhs, quat* rhs);
internal quat quat_sub(quat* lhs, quat* rhs);
internal quat quat_mul(quat* lhs, quat* rhs);
internal quat quat_mul_s(quat* lhs, f32 rhs);

internal quat quat_inv(quat* q);
internal quat quat_conj(quat* q);

internal f32 quat_dot(quat* lhs, quat* rhs);
internal f32 quat_len(quat* q);
internal f32 quat_len_sqr(quat* q);

internal vec3 quat_to_euler(quat* q);
internal quat quat_from_euler(vec3* angles);
internal quat quat_from_matrix(mat4* matrix);
internal quat quat_from_axis_angle(vec3* axis, f32 angle);

#endif
