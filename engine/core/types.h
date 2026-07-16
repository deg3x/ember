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

#define TEXTURE_TYPE_ALBEDO              0
#define TEXTURE_TYPE_METALIC_ROUGHNESS   1
#define TEXTURE_TYPE_NORMAL              2
#define TEXTURE_TYPE_AMBIENT_OCCLUSION   3
#define TEXTURE_TYPE_EMISSION            4

#define color3_t vec3_t
#define color4_t vec4_t

typedef u32 coord_space_t;
enum
{
    COORD_SPACE_local,
    COORD_SPACE_world
};

typedef struct camera_t camera_t;
struct camera_t
{
    f32 fov;
    f32 aspect;
    f32 clip_near;
    f32 clip_far;
    f32 clip_left;
    f32 clip_right;
    f32 clip_bottom;
    f32 clip_top;
};

typedef struct vertex_t vertex_t;
struct vertex_t
{
    vec3_t position;
    vec3_t normal;
    vec4_t tangent;
    vec3_t color;
    vec2_t uv0;
    vec2_t uv1;
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

    b32      use_tex_al;
    b32      use_tex_mr;
    b32      use_tex_nm;
    b32      use_tex_ao;
    b32      use_tex_em;

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

    i32      _padding[3];
};

#endif // TYPES_H
