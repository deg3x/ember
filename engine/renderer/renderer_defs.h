#ifndef RENDERER_DEFS_H 
#define RENDERER_DEFS_H 

#define RENDERER_SIZE_VERTEX sizeof(vertex_t)
#define RENDERER_SIZE_INDEX  sizeof(u32)

#define RENDERER_SWAP_IMG_COUNT   2
#define RENDERER_FRAMES_IN_FLIGHT 2

#define RENDERER_MESH_COUNT_MAX    8192
#define RENDERER_NODE_COUNT_MAX    8192
#define RENDERER_TEX_COUNT_MAX     4096
#define RENDERER_MAT_COUNT_MAX     512
#define RENDERER_SAMPLER_COUNT_MAX 32

#define FRAME_SIZE(s) ((s) / RENDERER_FRAMES_IN_FLIGHT)

typedef u32 renderer_vattr_type_t;
enum
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
    i32 material_id;
};

typedef struct renderer_tex_t renderer_tex_t;
struct renderer_tex_t
{
    i32 id;
    i32 image_id;
    i32 sampler_id;
};

typedef u32 renderer_img_tiling_t;
enum
{
    RENDERER_IMG_TILING_linear  = VK_IMAGE_TILING_LINEAR,
    RENDERER_IMG_TILING_optimal = VK_IMAGE_TILING_OPTIMAL,
};

typedef u32 renderer_img_format_t;
enum
{
    RENDERER_IMG_FORMAT_rgba_srgb = VK_FORMAT_R8G8B8A8_SRGB,
};

typedef u32 renderer_img_usage_flags_t;
enum
{
    RENDERER_IMG_USAGE_FLAGS_transfer_src  = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
    RENDERER_IMG_USAGE_FLAGS_transfer_dst  = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    RENDERER_IMG_USAGE_FLAGS_depth_stencil = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    RENDERER_IMG_USAGE_FLAGS_sampled       = VK_IMAGE_USAGE_SAMPLED_BIT,
    RENDERER_IMG_USAGE_FLAGS_color         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
};

typedef struct renderer_tex_info_t renderer_tex_info_t;
struct renderer_tex_info_t
{
    u32                        width;
    u32                        height;
    u8*                        pixels;
    i32                        pixels_size;
    renderer_img_format_t      format;
    renderer_img_tiling_t      tiling;
    renderer_img_usage_flags_t usage_flags;
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
    i32 transform_id;
    i32 material_id;
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
    VK_KHR_MAINTENANCE1_EXTENSION_NAME,
};

#endif // RENDERER_DEFS_H
