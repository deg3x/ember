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

#define color3 vec3
#define color4 vec4

typedef u32 coord_space;
enum
{
    COORD_SPACE_local,
    COORD_SPACE_world
};

typedef struct vertex vertex;
struct vertex
{
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;
};

typedef struct mesh mesh;
struct mesh
{
    vertex* vertices;
    i32*    indices;
    i32     vertex_count;
    i32     index_count;
};

typedef struct tex_sampler tex_sampler;
struct tex_sampler
{
    i8 min_filter;
    i8 mag_filter;
    i8 wrap_u;
    i8 wrap_v;
};

typedef struct texture texture;
struct texture
{
    u8*         data;
    i32         type;
    i32         size;
    tex_sampler sampler;
};

typedef struct material material;
struct material
{
    color4 color;
    color3 color_em;

    f32    metal;
    f32    rough;
    f32    ao;
    i32    alpha_mode;
    f32    alpha_cutoff;
    f32    normal_scale;
    b32    double_sided;

    i32    tex_id_al;
    i32    tex_id_mr;
    i32    tex_id_nm;
    i32    tex_id_ao;
    i32    tex_id_em;

    i32    tex_uv_al;
    i32    tex_uv_mr;
    i32    tex_uv_nm;
    i32    tex_uv_ao;
    i32    tex_uv_em;
};

#endif // TYPES_H
