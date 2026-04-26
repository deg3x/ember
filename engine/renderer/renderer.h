#ifndef RENDERER_H 
#define RENDERER_H 

#define RENDERER_SIZE_VERTEX sizeof(vertex_t)
#define RENDERER_SIZE_INDEX  sizeof(u32)

#define RENDERER_SWAP_IMG_COUNT 2
#define RENDERER_FRAMES_IN_FLIGHT 2

#define RENDERER_MESH_COUNT_MAX 10000
#define RENDERER_NODE_COUNT_MAX 10000

#define FRAME_SIZE(s) ((s) / RENDERER_FRAMES_IN_FLIGHT)

typedef enum vertex_attr_type_t vertex_attr_type_t;
enum vertex_attr_type_t
{
    VERTEX_ATTR_TYPE_position,
    VERTEX_ATTR_TYPE_normal,
    VERTEX_ATTR_TYPE_color,
    VERTEX_ATTR_TYPE_uv,
    VERTEX_ATTR_TYPE_count
};

typedef struct renderer_queue_ids_t renderer_queue_ids_t;
struct renderer_queue_ids_t
{
    u32 graphics;
    u32 presentation;
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

typedef struct renderer_push_constant_t renderer_push_constant_t;
struct renderer_push_constant_t
{
    u32 transform_id;
};

typedef struct renderer_draw_data_t renderer_draw_data_t;
struct renderer_draw_data_t
{
    u32 transform_id;
};

typedef VkDrawIndexedIndirectCommand renderer_dcmd_data_t;

typedef struct renderer_buffers_t renderer_buffers_t;
struct renderer_buffers_t
{
    VkBuffer    vertex_buf;
    VkBuffer    index_buf;
    VkBuffer    stage_buf;
    VkBuffer    ubo_buf;
    VkBuffer    ssbo_buf;
    VkBuffer    dcmd_buf;
    VkBuffer    draw_buf;

    VkImage     depth_image;
    VkImageView depth_image_view;

    gpu_mem_t*  vertex_mem;
    gpu_mem_t*  index_mem;
    gpu_mem_t*  stage_mem;
    gpu_mem_t*  ubo_mem;
    gpu_mem_t*  ssbo_mem;
    gpu_mem_t*  dcmd_mem;
    gpu_mem_t*  draw_mem;
    gpu_mem_t*  depth_mem;

    void*       stage_mapped;
    void*       ubo_mapped;
    void*       ssbo_mapped;
    void*       dcmd_mapped;
    void*       draw_mapped;
};

typedef struct renderer_mesh_t renderer_mesh_t;
struct renderer_mesh_t
{
    u32 vertex_offset;
    u32 vertex_count;
    i32 index_offset;
    u32 index_count;
};

typedef struct renderer_node_t renderer_node_t;
struct renderer_node_t
{
    i32 mesh_id;
    i32 mesh_count;
};

typedef struct renderer_pipeline_t renderer_pipeline_t;
struct renderer_pipeline_t
{
    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorSet       descriptor_sets[RENDERER_FRAMES_IN_FLIGHT];
    VkPipelineLayout      graphics_pipeline_layout;
    VkPipeline            graphics_pipeline;
};

typedef struct renderer_t renderer_t;
struct renderer_t
{
    gpu_arena_t          gpu_arena;
    cpu_arena_t*         host_arena;
    VkInstance           instance;
    VkSurfaceKHR         surface;
    VkPhysicalDevice     physical_device;
    VkDevice             device;
    renderer_queue_ids_t queue_ids;
    VkQueue              graphics_queue;
    VkQueue              present_queue;
    VkSwapchainKHR       swapchain;
    VkImage*             swapchain_images;
    VkImageView*         swapchain_img_views;
    VkExtent2D           swapchain_extent;
    VkFormat             swapchain_img_fmt;
    VkCommandPool        command_pool;
    VkDescriptorPool     descriptor_pool;
    VkCommandBuffer      command_buffers[RENDERER_FRAMES_IN_FLIGHT];
    VkSemaphore          sem_img_avail[RENDERER_FRAMES_IN_FLIGHT];
    VkSemaphore          sem_render_end[RENDERER_FRAMES_IN_FLIGHT];
    VkFence              fence_in_flight[RENDERER_FRAMES_IN_FLIGHT];

    renderer_buffers_t   buffers;

    renderer_pipeline_t* pipelines;
    renderer_mesh_t*     mesh_data;
    renderer_node_t*     node_data;

    i32                  pipeline_count;
    i32                  mesh_count;
    i32                  node_count;
};

global renderer_t g_renderer = {0};

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

internal void renderer_init(platform_hnd_t window_handle);
internal void renderer_update(platform_hnd_t window_handle);
internal void renderer_destroy();

internal void renderer_pipeline_init(renderer_pipeline_t* state);
internal void renderer_pipeline_destroy(renderer_pipeline_t* state);

internal void renderer_create_instance();
internal void renderer_create_surface(platform_hnd_t window_handle);
internal void renderer_create_physical_device();
internal void renderer_create_queue_ids();
internal void renderer_create_device();
internal void renderer_create_swapchain(platform_hnd_t window_handle);
internal void renderer_create_command_pool();
internal void renderer_create_command_buffers();
internal void renderer_create_descriptor_pool();
internal void renderer_create_sync_primitives();
internal void renderer_create_resources();

internal void renderer_create_nodes(renderer_node_t* nodes, renderer_ssbo_t* node_ssbo, i32 node_count);
internal void renderer_create_meshes(mesh_t* m, i32 count);

internal void renderer_create_depth_resources();
internal void renderer_create_buffer(VkBuffer* buffer, VkDeviceSize size, VkBufferUsageFlags usage);
internal void renderer_create_image(VkImage* image, u32 width, u32 height, VkImageUsageFlags usage, VkImageTiling tiling, VkFormat format);
internal void renderer_create_image_view(VkImage image, VkImageView* view, VkFormat format, VkImageAspectFlags aspect_flags);
internal void renderer_copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize offset_src, VkDeviceSize offset_dst, VkDeviceSize size);

internal gpu_mem_t* renderer_create_buffer_memory(VkBuffer buffer, gpu_mem_type_t mem_type);
internal gpu_mem_t* renderer_create_image_memory(VkImage image, gpu_mem_type_t mem_type);

internal void           renderer_pipeline_create_descriptor_set_layout(renderer_pipeline_t* pipeline);
internal void           renderer_pipeline_create_descriptor_sets(renderer_pipeline_t* pipeline);
internal void           renderer_pipeline_create_graphics_pipeline_layout(renderer_pipeline_t* pipeline);
internal void           renderer_pipeline_create_graphics_pipeline(renderer_pipeline_t* pipeline);
internal VkShaderModule renderer_pipeline_create_shader_module(const u8* code, u64 code_size);

internal void               renderer_swapchain_recreate(platform_hnd_t window_handle);
internal VkSurfaceFormatKHR renderer_swapchain_find_format();
internal VkPresentModeKHR   renderer_swapchain_find_present();
internal VkExtent2D         renderer_swapchain_find_extent(platform_hnd_t window_handle);

internal void renderer_command_buffer_record(renderer_pipeline_t* pipeline, u32 buffer_id, u32 img_id);

internal b32 renderer_check_validation_layers();
internal b32 renderer_check_instance_extensions();
internal b32 renderer_check_device_extensions(VkPhysicalDevice device);
internal b32 renderer_device_is_suitable(VkPhysicalDevice device);

#endif // RENDERER_H 
