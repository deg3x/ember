#ifndef PARSER_GLTF_H
#define PARSER_GLTF_H

#define GLTF_MAGIC                0x46546C67
#define GLTF_CHUNK_JSON           0x4E4F534A
#define GLTF_CHUNK_BIN            0x004E4942
#define GLTF_BYTE                 5120
#define GLTF_UBYTE                5121
#define GLTF_SHORT                5122
#define GLTF_USHORT               5123
#define GLTF_UINT                 5125
#define GLTF_FLOAT                5126
#define GLTF_POINTS               0
#define GLTF_LINES                1
#define GLTF_LINE_LOOP            2
#define GLTF_LINE_STRIP           3
#define GLTF_TRIANGLES            4
#define GLTF_TRIANGLE_STRIP       5
#define GLTF_TRIANGLE_FAN         6
#define GLTF_ARRAY_BUFFER         34962
#define GLTF_ELEMENT_ARRAY_BUFFER 34963

typedef u32_t gltf_accessor_type_t;
enum
{
    GLTF_ACCESSOR_TYPE_scalar,
    GLTF_ACCESSOR_TYPE_vec2,
    GLTF_ACCESSOR_TYPE_vec3,
    GLTF_ACCESSOR_TYPE_vec4,
    GLTF_ACCESSOR_TYPE_mat2,
    GLTF_ACCESSOR_TYPE_mat3,
    GLTF_ACCESSOR_TYPE_mat4,
};

typedef struct gltf_parser_t gltf_parser_t;
struct gltf_parser_t
{
    arena_t* arena;
    buffer_t source;
    u64_t    position;
};

typedef struct gltf_primitives_t gltf_primitives_t;
struct gltf_primitives_t
{
    i32_t              position;
    i32_t              normal;
    i32_t              tangent;
    i32_t              texcoord;
    i32_t              color;
    i32_t              indices;
    i32_t              material;
    i32_t              draw_mode;
    gltf_primitives_t* next;
};

typedef struct gltf_meshes_t gltf_meshes_t;
struct gltf_meshes_t
{
    buffer_t          name;
    gltf_primitives_t first_primitives;
};

typedef struct gltf_accessors_t gltf_accessors_t;
struct gltf_accessors_t
{
    i32_t                buffer_view_id;
    i32_t                byte_offset;
    i32_t                component_type;
    i32_t                count;
    b32_t                normalized;
    buffer_t             min;
    buffer_t             max;
    gltf_accessor_type_t type;
};

typedef struct gltf_buffer_views_t gltf_buffer_views_t;
struct gltf_buffer_views_t
{
    i32_t buffer_id;
    i32_t byte_offset;
    i32_t byte_length;
    i32_t byte_stride;
    i32_t target;
};

typedef struct gltf_buffers_t gltf_buffers_t;
struct gltf_buffers_t
{
    i32_t byte_length;
};

typedef struct gltf_json_data_t gltf_json_data_t;
struct gltf_json_data_t
{
    gltf_buffers_t*      buffers;
    gltf_buffer_views_t* buffer_views;
    gltf_accessors_t*    accessors;
    gltf_meshes_t*       meshes;
};

typedef struct gltf_data_t gltf_data_t;
struct gltf_data_t
{
    vec3_t* vertices;
    vec3_t* normals;
    u32_t*  indices;
};

internal void gltf_parse_file(const char* file_path, arena_t* arena);
internal void gltf_parse_chunk_json(gltf_parser_t* parser, u32_t chunk_length);
internal void gltf_parse_chunk_binary(gltf_parser_t* parser, u32_t chunk_length);

#endif //PARSER_GLTF_H
