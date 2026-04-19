#ifndef PARSER_GLTF_H
#define PARSER_GLTF_H

#define GLTF_MAGIC                    0x46546C67
#define GLTF_CHUNK_JSON               0x4E4F534A
#define GLTF_CHUNK_BIN                0x004E4942
#define GLTF_COMPONENT_TYPE_BYTE      5120
#define GLTF_COMPONENT_TYPE_UBYTE     5121
#define GLTF_COMPONENT_TYPE_SHORT     5122
#define GLTF_COMPONENT_TYPE_USHORT    5123
#define GLTF_COMPONENT_TYPE_UINT      5125
#define GLTF_COMPONENT_TYPE_FLOAT     5126
#define GLTF_DRAW_MODE_POINTS         0
#define GLTF_DRAW_MODE_LINES          1
#define GLTF_DRAW_MODE_LINE_LOOP      2
#define GLTF_DRAW_MODE_LINE_STRIP     3
#define GLTF_DRAW_MODE_TRIANGLES      4
#define GLTF_DRAW_MODE_TRIANGLE_STRIP 5
#define GLTF_DRAW_MODE_TRIANGLE_FAN   6
#define GLTF_ARRAY_BUFFER             34962
#define GLTF_ELEMENT_ARRAY_BUFFER     34963

#define GLTF_CUSTOM_TYPE_SCALAR       100
#define GLTF_CUSTOM_TYPE_VEC2         101
#define GLTF_CUSTOM_TYPE_VEC3         102
#define GLTF_CUSTOM_TYPE_VEC4         103
#define GLTF_CUSTOM_TYPE_MAT2         104
#define GLTF_CUSTOM_TYPE_MAT3         105
#define GLTF_CUSTOM_TYPE_MAT4         106

#define GLTF_MIME_TYPE_PNG            0
#define GLTF_MIME_TYPE_JPEG           1

#define GLTF_MAG_FILTER_NEAR          9728
#define GLTF_MAG_FILTER_LIN           9729

#define GLTF_MIN_FILTER_NEAR          9728
#define GLTF_MIN_FILTER_LIN           9729
#define GLTF_MIN_FILTER_NEAR_MIP_NEAR 9984
#define GLTF_MIN_FILTER_LIN_MIP_NEAR  9985
#define GLTF_MIN_FILTER_NEAR_MIP_LIN  9986
#define GLTF_MIN_FILTER_LIN_MIP_LIN   9987

#define GLTF_WRAP_REPEAT              10497
#define GLTF_WRAP_CLAMP_TO_EDGE       33071
#define GLTF_WRAP_MIRRORED_REPEAT     33648

#define GLTF_ALPHA_MODE_OPAQUE 0
#define GLTF_ALPHA_MODE_MASK   1
#define GLTF_ALPHA_MODE_BLEND  2

typedef i32 gltf_mime_type;
enum
{
    GLTF_MIME_TYPE_png,
    GLTF_MIME_TYPE_jpeg,
};

typedef struct gltf_parser gltf_parser;
struct gltf_parser
{
    cpu_arena* arena;
    buffer     source;
    u64        position;
};

typedef struct gltf_primitive gltf_primitive;
struct gltf_primitive
{
    i32 position;
    i32 normal;
    i32 tangent;
    i32 texcoord;
    i32 color;
    i32 joints;
    i32 weights;
    i32 indices;
    i32 material;
    i32 draw_mode;
};

typedef struct gltf_node gltf_node;
struct gltf_node
{
    mat4 matrix;
    i32* children;
    c8   name[64];
    i32  parent;
    b32  has_parent;
    i32  child_count;
    i32  mesh;
};

typedef struct gltf_mesh gltf_mesh;
struct gltf_mesh
{
    buffer          name;
    gltf_primitive* primitives;
    i32             primitive_count;
};

typedef struct gltf_accessor gltf_accessor;
struct gltf_accessor
{
    i32    buffer_view_id;
    i32    byte_offset;
    i32    component_type;
    i32    count;
    b32    normalized;
    i32    type;
    buffer min;
    buffer max;
};

typedef struct gltf_buffer_view gltf_buffer_view;
struct gltf_buffer_view
{
    i32 buffer_id;
    i32 byte_offset;
    i32 byte_length;
    i32 byte_stride;
    i32 target;
};

typedef struct gltf_buffer gltf_buffer;
struct gltf_buffer
{
    i32 byte_length;
};

typedef struct gltf_texture gltf_texture;
struct gltf_texture
{
    i32 sampler;
    i32 image;
};

typedef struct gltf_image gltf_image;
struct gltf_image
{
    i32            buffer_view_id;
    gltf_mime_type mime_type;
};

typedef struct gltf_sampler gltf_sampler;
struct gltf_sampler
{
    i32 min_filter;
    i32 mag_filter;
    i32 wrap_u;
    i32 wrap_v;
};

typedef struct gltf_material gltf_material;
struct gltf_material
{
    f32 pbr_clr[4];
    i32 pbr_tex_clr_id;
    i32 pbr_tex_clr_uv;
    f32 pbr_metal;
    f32 pbr_rough;
    i32 pbr_tex_mr_id;
    i32 pbr_tex_mr_uv;

    i32 tex_nm_id;
    i32 tex_nm_uv;
    f32 tex_nm_scale;

    i32 tex_ao_id;
    i32 tex_ao_uv;
    f32 tex_ao_str;

    i32 tex_em_id;
    i32 tex_em_uv;

    f32 emissive[3];
    i32 alpha_mode;
    f32 alpha_cutoff;
    b32 double_sided;
};

typedef struct gltf_json_data gltf_json_data;
struct gltf_json_data
{
    gltf_node*        nodes;
    gltf_mesh*        meshes;
    gltf_accessor*    accessors;
    gltf_buffer_view* buffer_views;
    gltf_buffer*      buffers;
    gltf_texture*     textures;
    gltf_image*       images;
    gltf_sampler*     samplers;
    gltf_material*    materials;

    i32               node_count;
    i32               mesh_count;
    i32               primitive_count;
    i32               accessor_count;
    i32               buffer_view_count;
    i32               buffer_count;
    i32               texture_count;
    i32               image_count;
    i32               sampler_count;
    i32               material_count;
};

typedef struct gltf_data gltf_data;
struct gltf_data
{
    cpu_arena* arena;

    mat4*      transforms;
    i32**      children;
    i32*       children_count;
    i32*       parents;

    i32*       mesh_ids;
    i32*       mesh_offsets;
    i32*       mesh_primitives;
    mesh*      meshes;
    texture*   textures;
    material*  materials;

    i32        node_count;
    i32        mesh_count;
    i32        primitive_count;
    i32        texture_count;
    i32        material_count;
};

gltf_data      gltf_parse_file(const c8* file_path);
gltf_json_data gltf_parse_chunk_json(gltf_parser* parser, u32 chunk_length);
gltf_data      gltf_parse_chunk_binary(gltf_parser* parser, u32 chunk_length, gltf_json_data* json_data);
void           gltf_parse_components(void* source, void* dest, i32 count, i32 offset, i32 stride, i32 cmp_type, i32 data_type);
void           gltf_free(gltf_data* gltf);

#endif //PARSER_GLTF_H
