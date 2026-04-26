#ifndef RENDERER_DEFS_H 
#define RENDERER_DEFS_H 

#define RENDERER_SIZE_VERTEX sizeof(vertex_t)
#define RENDERER_SIZE_INDEX  sizeof(u32)

#define RENDERER_SWAP_IMG_COUNT 2
#define RENDERER_FRAMES_IN_FLIGHT 2

#define RENDERER_MESH_COUNT_MAX 10000
#define RENDERER_NODE_COUNT_MAX 10000

#define FRAME_SIZE(s) ((s) / RENDERER_FRAMES_IN_FLIGHT)

typedef enum renderer_vattr_type_t renderer_vattr_type_t;
enum renderer_vattr_type_t
{
    RENDERER_VATTR_TYPE_position,
    RENDERER_VATTR_TYPE_normal,
    RENDERER_VATTR_TYPE_color,
    RENDERER_VATTR_TYPE_uv,
    RENDERER_VATTR_TYPE_count
};

typedef struct renderer_node_t renderer_node_t;
struct renderer_node_t
{
    i32 mesh_id;
    i32 mesh_count;
};

typedef struct renderer_mesh_t renderer_mesh_t;
struct renderer_mesh_t
{
    u32 vertex_offset;
    u32 vertex_count;
    i32 index_offset;
    u32 index_count;
};

typedef struct renderer_ubo_t renderer_ubo_t;
struct renderer_ubo_t
{
    mat4_t view;
    mat4_t proj;
};

typedef struct renderer_ssbo_t renderer_ssbo_t;
struct renderer_ssbo_t
{
    mat4_t transform;
};

typedef struct renderer_draw_data_t renderer_draw_data_t;
struct renderer_draw_data_t
{
    u32 transform_id;
};

typedef VkDrawIndexedIndirectCommand renderer_dcmd_data_t;

global const c8* g_validation_layers[] = {
    "VK_LAYER_KHRONOS_validation",
};

global const c8* g_instance_extensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#if PLATFORM_WINDOWS
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
};

global const c8* g_device_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

#endif // RENDERER_DEFS_H
