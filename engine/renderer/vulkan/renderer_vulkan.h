#ifndef RENDERER_VULKAN_H 
#define RENDERER_VULKAN_H 

#define RENDERER_SIZE_VERTEX sizeof(vertex_t)
#define RENDERER_SIZE_INDEX  sizeof(u32_t)

#define RENDERER_SWAP_IMG_COUNT 2
#define RENDERER_FRAMES_IN_FLIGHT 2

typedef enum vertex_attr_type_t vertex_attr_type_t;
enum vertex_attr_type_t
{
    VERTEX_ATTR_TYPE_position,
    VERTEX_ATTR_TYPE_normal,
    VERTEX_ATTR_TYPE_color,
    VERTEX_ATTR_TYPE_uv,
    VERTEX_ATTR_TYPE_count
};

typedef struct vertex_t vertex_t;
struct vertex_t
{
    vec3_t position;
    vec3_t normal;
    vec3_t color;
    vec2_t uv;
};

typedef struct mesh_t mesh_t;
struct mesh_t
{
    vertex_t* vertices;
    u32_t*    indices;
    u32_t     vertex_count;
    u32_t     index_count;
};

typedef struct renderer_queue_ids_t renderer_queue_ids_t;
struct renderer_queue_ids_t
{
    u32_t graphics;
    u32_t presentation;
};

typedef struct renderer_ubo_t renderer_ubo_t;
struct renderer_ubo_t
{
    mat4_t model;
    mat4_t view;
    mat4_t proj;
};

typedef struct renderer_buffers_t renderer_buffers_t;
struct renderer_buffers_t
{
    VkBuffer    vertex_buf;
    VkBuffer    index_buf;
    VkBuffer    stage_buf;
    VkBuffer    ubo_buf[RENDERER_FRAMES_IN_FLIGHT];

    VkImage     depth_image;
    VkImageView depth_image_view;

    gpu_mem_t*  vertex_mem;
    gpu_mem_t*  index_mem;
    gpu_mem_t*  stage_mem;
    gpu_mem_t*  ubo_mem[RENDERER_FRAMES_IN_FLIGHT];
    gpu_mem_t*  depth_mem;

    void*       stage_mapped;
    void*       ubo_mapped[RENDERER_FRAMES_IN_FLIGHT];
};

typedef struct renderer_mesh_t renderer_mesh_t;
struct renderer_mesh_t
{
    renderer_mesh_t* next;

    u32_t            vertex_offset;
    u32_t            vertex_count;
    i32_t            index_offset;
    u32_t            index_count;
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
    gpu_arena_t             gpu_arena;
    renderer_mesh_t*        mesh_data;
    arena_t*                host_arena;
    VkInstance              instance;
    VkSurfaceKHR            surface;
    VkPhysicalDevice        physical_device;
    VkDevice                device;
    renderer_queue_ids_t queue_ids;
    VkQueue                 graphics_queue;
    VkQueue                 present_queue;
    VkSwapchainKHR          swapchain;
    VkImage*                swapchain_images;
    VkImageView*            swapchain_img_views;
    VkExtent2D              swapchain_extent;
    VkFormat                swapchain_img_fmt;
    VkCommandPool           command_pool;
    VkDescriptorPool        descriptor_pool;
    VkCommandBuffer         command_buffers[RENDERER_FRAMES_IN_FLIGHT];
    VkSemaphore             sem_img_avail[RENDERER_FRAMES_IN_FLIGHT];
    VkSemaphore             sem_render_end[RENDERER_FRAMES_IN_FLIGHT];
    VkFence                 fence_in_flight[RENDERER_FRAMES_IN_FLIGHT];

    renderer_buffers_t      buffers;

    renderer_pipeline_t*    pipelines;
    u64_t                   pipeline_count;
};

global renderer_t g_renderer;

global const char* g_validation_layers[] = {
    "VK_LAYER_KHRONOS_validation",
};

global const char* g_instance_extensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#if PLATFORM_WINDOWS
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
};

global const char* g_device_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};


internal void renderer_init(platform_handle_t window_handle);
internal void renderer_update(platform_handle_t window_handle);
internal void renderer_destroy();

internal void renderer_pipeline_init(renderer_pipeline_t* state);
internal void renderer_pipeline_destroy(renderer_pipeline_t* state);

internal void renderer_create_instance();
internal void renderer_create_surface(platform_handle_t window_handle);
internal void renderer_create_physical_device();
internal void renderer_create_queue_ids();
internal void renderer_create_device();
internal void renderer_create_swapchain(platform_handle_t window_handle);
internal void renderer_create_command_pool();
internal void renderer_create_command_buffers();
internal void renderer_create_descriptor_pool();
internal void renderer_create_sync_primitives();
internal void renderer_create_resources();

internal void renderer_create_mesh(vertex_t* vertices, u32_t vertex_count, u32_t* indices, u32_t index_count);
internal void renderer_create_depth_resources();
internal void renderer_create_buffer(VkBuffer* buffer, VkDeviceSize size, VkBufferUsageFlags usage);
internal void renderer_create_image(VkImage* image, u32_t width, u32_t height, VkImageUsageFlags usage, VkImageTiling tiling, VkFormat format);
internal void renderer_create_image_view(VkImage image, VkImageView* view, VkFormat format, VkImageAspectFlags aspect_flags);
internal void renderer_copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize offset_src, VkDeviceSize offset_dst, VkDeviceSize size);

internal gpu_mem_t* renderer_create_buffer_memory(VkBuffer buffer, gpu_mem_type_t mem_type);
internal gpu_mem_t* renderer_create_image_memory(VkImage image, gpu_mem_type_t mem_type);

internal void           renderer_pipeline_create_descriptor_set_layout(renderer_pipeline_t* pipeline);
internal void           renderer_pipeline_create_descriptor_sets(renderer_pipeline_t* pipeline);
internal void           renderer_pipeline_create_graphics_pipeline_layout(renderer_pipeline_t* pipeline);
internal void           renderer_pipeline_create_graphics_pipeline(renderer_pipeline_t* pipeline);
internal VkShaderModule renderer_pipeline_create_shader_module(const u8_t* code, u64_t code_size);

internal void               renderer_swapchain_recreate(platform_handle_t window_handle);
internal VkSurfaceFormatKHR renderer_swapchain_find_format();
internal VkPresentModeKHR   renderer_swapchain_find_present();
internal VkExtent2D         renderer_swapchain_find_extent(platform_handle_t window_handle);

internal void renderer_command_buffer_record(renderer_pipeline_t* pipeline, u32_t buffer_id, u32_t img_id);

internal b32_t renderer_check_validation_layers();
internal b32_t renderer_check_instance_extensions();
internal b32_t renderer_check_device_extensions(VkPhysicalDevice device);
internal b32_t renderer_device_is_suitable(VkPhysicalDevice device);

#endif // RENDERER_VULKAN_H 
