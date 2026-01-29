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

typedef struct gltf_parser_t gltf_parser_t;
struct gltf_parser_t
{
    arena_t* arena;
    buffer_t source;
    u64_t    position;
};

typedef struct gltf_primitive_t gltf_primitive_t;
struct gltf_primitive_t
{
    i32_t             position;
    i32_t             normal;
    i32_t             tangent;
    i32_t             texcoord;
    i32_t             color;
    i32_t             joints;
    i32_t             weights;
    i32_t             indices;
    i32_t             material;
    i32_t             draw_mode;
    gltf_primitive_t* next;
};

typedef struct gltf_mesh_t gltf_mesh_t;
struct gltf_mesh_t
{
    buffer_t         name;
    gltf_primitive_t first_primitive;
};

typedef struct gltf_accessor_t gltf_accessor_t;
struct gltf_accessor_t
{
    i32_t    buffer_view_id;
    i32_t    byte_offset;
    i32_t    component_type;
    i32_t    count;
    b32_t    normalized;
    i32_t    type;
    buffer_t min;
    buffer_t max;
};

typedef struct gltf_buffer_view_t gltf_buffer_view_t;
struct gltf_buffer_view_t
{
    i32_t buffer_id;
    i32_t byte_offset;
    i32_t byte_length;
    i32_t byte_stride;
    i32_t target;
};

typedef struct gltf_buffer_t gltf_buffer_t;
struct gltf_buffer_t
{
    i32_t byte_length;
};

typedef struct gltf_json_data_t gltf_json_data_t;
struct gltf_json_data_t
{
    gltf_mesh_t*        meshes;
    gltf_accessor_t*    accessors;
    gltf_buffer_view_t* buffer_views;
    gltf_buffer_t*      buffers;
    u32_t               mesh_count;
    u32_t               accessor_count;
    u32_t               buffer_view_count;
    u32_t               buffer_count;
};

typedef struct gltf_data_t gltf_data_t;
struct gltf_data_t
{
    //vertex_t* vertices;
    //u32_t*    indices;
    //u32_t     vertex_count;
    //u32_t     index_count;

    mesh_t* meshes;
    u32_t   mesh_count;
};

internal gltf_data_t      gltf_parse_file(const char* file_path, arena_t* arena);
internal gltf_json_data_t gltf_parse_chunk_json(gltf_parser_t* parser, u32_t chunk_length);
internal gltf_data_t      gltf_parse_chunk_binary(gltf_parser_t* parser, u32_t chunk_length, gltf_json_data_t* json_data);
internal void             gltf_parse_components(void* source, u32_t count, u32_t offset, u32_t stride, i32_t cmp_type, i32_t data_type, void* dest);

#endif //PARSER_GLTF_H
