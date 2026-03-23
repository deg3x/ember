#ifndef VECTOR_H
#define VECTOR_H

vec2 vec2_norm(vec2* v);
vec3 vec3_norm(vec3* v);
vec4 vec4_norm(vec4* v);

vec2 vec2_neg(vec2* v);
vec3 vec3_neg(vec3* v);
vec4 vec4_neg(vec4* v);

vec2 vec2_add(vec2* lhs, vec2* rhs);
vec3 vec3_add(vec3* lhs, vec3* rhs);
vec4 vec4_add(vec4* lhs, vec4* rhs);
vec2 vec2_sub(vec2* lhs, vec2* rhs);
vec3 vec3_sub(vec3* lhs, vec3* rhs);
vec4 vec4_sub(vec4* lhs, vec4* rhs);
vec2 vec2_mul(vec2* lhs, vec2* rhs);
vec3 vec3_mul(vec3* lhs, vec3* rhs);
vec4 vec4_mul(vec4* lhs, vec4* rhs);
vec2 vec2_mul_s(vec2* lhs, f32 rhs);
vec3 vec3_mul_s(vec3* lhs, f32 rhs);
vec4 vec4_mul_s(vec4* lhs, f32 rhs);

f32 vec2_dot(vec2* lhs, vec2* rhs);
f32 vec3_dot(vec3* lhs, vec3* rhs);
f32 vec4_dot(vec4* lhs, vec4* rhs);

f32 vec2_len(vec2* v);
f32 vec3_len(vec3* v);
f32 vec4_len(vec4* v);

f32 vec2_len_sqr(vec2* v);
f32 vec3_len_sqr(vec3* v);
f32 vec4_len_sqr(vec4* v);

vec3 vec3_cross(vec3* lhs, vec3* rhs);
vec3 vec3_rotate_quat(vec3* v, quat* q);
vec3 vec3_rotate_axis(vec3* v, vec3* axis, f32 angle);

#endif // VECTOR_H
