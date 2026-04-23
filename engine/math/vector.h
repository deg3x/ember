#ifndef VECTOR_H
#define VECTOR_H

internal vec2_t vec2_norm(vec2_t* v);
internal vec3_t vec3_norm(vec3_t* v);
internal vec4_t vec4_norm(vec4_t* v);

internal vec2_t vec2_neg(vec2_t* v);
internal vec3_t vec3_neg(vec3_t* v);
internal vec4_t vec4_neg(vec4_t* v);

internal vec2_t vec2_add(vec2_t* lhs, vec2_t* rhs);
internal vec3_t vec3_add(vec3_t* lhs, vec3_t* rhs);
internal vec4_t vec4_add(vec4_t* lhs, vec4_t* rhs);
internal vec2_t vec2_sub(vec2_t* lhs, vec2_t* rhs);
internal vec3_t vec3_sub(vec3_t* lhs, vec3_t* rhs);
internal vec4_t vec4_sub(vec4_t* lhs, vec4_t* rhs);
internal vec2_t vec2_mul(vec2_t* lhs, vec2_t* rhs);
internal vec3_t vec3_mul(vec3_t* lhs, vec3_t* rhs);
internal vec4_t vec4_mul(vec4_t* lhs, vec4_t* rhs);
internal vec2_t vec2_mul_s(vec2_t* lhs, f32 rhs);
internal vec3_t vec3_mul_s(vec3_t* lhs, f32 rhs);
internal vec4_t vec4_mul_s(vec4_t* lhs, f32 rhs);

internal f32 vec2_dot(vec2_t* lhs, vec2_t* rhs);
internal f32 vec3_dot(vec3_t* lhs, vec3_t* rhs);
internal f32 vec4_dot(vec4_t* lhs, vec4_t* rhs);

internal f32 vec2_len(vec2_t* v);
internal f32 vec3_len(vec3_t* v);
internal f32 vec4_len(vec4_t* v);

internal f32 vec2_len_sqr(vec2_t* v);
internal f32 vec3_len_sqr(vec3_t* v);
internal f32 vec4_len_sqr(vec4_t* v);

internal vec3_t vec3_cross(vec3_t* lhs, vec3_t* rhs);
internal vec3_t vec3_rotate_quat(vec3_t* v, quat_t* q);
internal vec3_t vec3_rotate_axis(vec3_t* v, vec3_t* axis, f32 angle);

#endif // VECTOR_H
