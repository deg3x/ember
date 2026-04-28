#ifndef TYPES_H
#define TYPES_H

#define SAMPLER_MAG_FILTER_NEAR          0
#define SAMPLER_MAG_FILTER_LIN           1

#define SAMPLER_MIN_FILTER_NEAR          0
#define SAMPLER_MIN_FILTER_LIN           1
#define SAMPLER_MIN_FILTER_NEAR_MIP_NEAR 2
#define SAMPLER_MIN_FILTER_LIN_MIP_NEAR  3
#define SAMPLER_MIN_FILTER_NEAR_MIP_LIN  4
#define SAMPLER_MIN_FILTER_LIN_MIP_LIN   5

#define SAMPLER_WRAP_REPEAT              0
#define SAMPLER_WRAP_CLAMP_TO_EDGE       1
#define SAMPLER_WRAP_MIRRORED_REPEAT     2

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

typedef union quat_t quat_t;
union quat_t
{
    struct
    {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };

    f32 data[4];
};

// NOTE(KB): Matrices are stored in a column-first order
typedef union mat4_t mat4_t;
union mat4_t
{
    struct
    {
        vec4_t cols[4];
    };

    f32 m[4][4];
};

#pragma warning(pop)

#define VEC2_ZERO     (vec2_t){0.0f, 0.0f}
#define VEC3_ZERO     (vec3_t){0.0f, 0.0f, 0.0f}
#define VEC3_ONE      (vec3_t){1.0f, 1.0f, 1.0f}
#define VEC3_UP       (vec3_t){0.0f, 1.0f, 0.0f}
#define VEC3_RIGHT    (vec3_t){1.0f, 0.0f, 0.0f}
#define VEC3_FORWARD  (vec3_t){0.0f, 0.0f, 1.0f}
#define VEC4_ZERO     (vec4_t){0.0f, 0.0f, 0.0f, 0.0f}
#define QUAT_IDENTITY (quat_t){0.0f, 0.0f, 0.0f, 1.0f}
#define MAT4_IDENTITY (mat4_t)  \
    {                           \
        1.0f, 0.0f, 0.0f, 0.0f, \
        0.0f, 1.0f, 0.0f, 0.0f, \
        0.0f, 0.0f, 1.0f, 0.0f, \
        0.0f, 0.0f, 0.0f, 1.0f  \
    }

#define color3_t vec3_t
#define color4_t vec4_t

typedef u32 coord_space_t;
enum
{
    COORD_SPACE_local,
    COORD_SPACE_world
};

typedef struct vertex_t vertex_t;
struct vertex_t
{
    vec3_t position;
    vec3_t normal;
    vec3_t color;
    vec2_t uv;
};

typedef struct sampler_t sampler_t;
struct sampler_t
{
    i8 min_filter;
    i8 mag_filter;
    i8 wrap_u;
    i8 wrap_v;
};

typedef struct texture_t texture_t;
struct texture_t
{
    u8*       data;
    i32       type;
    i32       size;
    sampler_t sampler;
};

typedef struct material_t material_t;
struct material_t
{
    color4_t color;
    color3_t color_em;

    f32      metal;
    f32      rough;
    f32      ao;
    i32      alpha_mode;
    f32      alpha_cutoff;
    f32      normal_scale;
    b32      double_sided;

    i32      tex_id_al;
    i32      tex_id_mr;
    i32      tex_id_nm;
    i32      tex_id_ao;
    i32      tex_id_em;

    i32      tex_uv_al;
    i32      tex_uv_mr;
    i32      tex_uv_nm;
    i32      tex_uv_ao;
    i32      tex_uv_em;
};

#endif // TYPES_H
