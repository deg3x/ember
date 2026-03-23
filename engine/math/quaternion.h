#ifndef QUATERNION_H
#define QUATERNION_H

quat quat_norm(quat* q);
quat quat_renorm(quat* q);

quat quat_add(quat* lhs, quat* rhs);
quat quat_sub(quat* lhs, quat* rhs);
quat quat_mul(quat* lhs, quat* rhs);
quat quat_mul_s(quat* lhs, f32 rhs);

quat quat_inv(quat* q);
quat quat_conj(quat* q);

f32 quat_dot(quat* lhs, quat* rhs);
f32 quat_len(quat* q);
f32 quat_len_sqr(quat* q);

vec3 quat_to_euler(quat* q);
quat quat_from_euler(vec3* angles);
quat quat_from_matrix(mat4* matrix);
quat quat_from_axis_angle(vec3* axis, f32 angle);

#endif
