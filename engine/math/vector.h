#ifndef VECTOR_H
#define VECTOR_H

#pragma warning(push)
#pragma warning(disable: 4201)

typedef union vec2_t vec2_t;
union vec2_t
{
    struct
    {
        f32 x;
        f32 y;
    };

    struct
    {
        f32 u;
        f32 v;
    };

    f32 data[2];
};

typedef union vec3_t vec3_t;
union vec3_t
{
    struct
    {
        f32 x;
        f32 y;
        f32 z;
    };

    struct
    {
        f32 r;
        f32 g;
        f32 b;
    };

    struct
    {
        f32 u;
        f32 v;
        f32 w;
    };

    f32 data[3];
};

typedef union vec4_t vec4_t;
union vec4_t
{
    struct
    {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };

    struct
    {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };

    f32 data[4];
};

#pragma warning(pop)

#define VEC2_ZERO     (vec2_t){ 0.0f,  0.0f}
#define VEC3_ZERO     (vec3_t){ 0.0f,  0.0f,  0.0f}
#define VEC3_ONE      (vec3_t){ 1.0f,  1.0f,  1.0f}
#define VEC3_UP       (vec3_t){ 0.0f,  1.0f,  0.0f}
#define VEC3_DOWN     (vec3_t){ 0.0f, -1.0f,  0.0f}
#define VEC3_RIGHT    (vec3_t){ 1.0f,  0.0f,  0.0f}
#define VEC3_LEFT     (vec3_t){-1.0f,  0.0f,  0.0f}
#define VEC3_FORWARD  (vec3_t){ 0.0f,  0.0f,  1.0f}
#define VEC3_BACK     (vec3_t){ 0.0f,  0.0f, -1.0f}
#define VEC4_ZERO     (vec4_t){ 0.0f,  0.0f,  0.0f, 0.0f}

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

#endif // VECTOR_H
