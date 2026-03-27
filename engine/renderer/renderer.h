#ifndef RENDERER_H 
#define RENDERER_H 

#define RENDERER_SIZE_VERTEX sizeof(vertex)
#define RENDERER_SIZE_INDEX  sizeof(u32)

#define RENDERER_SWAP_IMG_COUNT 2
#define RENDERER_FRAMES_IN_FLIGHT 2

#define RENDERER_MESH_COUNT_MAX 10000

typedef enum vertex_attr_type vertex_attr_type;
enum vertex_attr_type
{
    VERTEX_ATTR_TYPE_position,
    VERTEX_ATTR_TYPE_normal,
    VERTEX_ATTR_TYPE_color,
    VERTEX_ATTR_TYPE_uv,
    VERTEX_ATTR_TYPE_count
};

typedef struct renderer_queue_ids renderer_queue_ids;
struct renderer_queue_ids
{
    u32 graphics;
    u32 presentation;
};

typedef struct renderer_ubo renderer_ubo;
struct renderer_ubo
{
    mat4 view;
    mat4 proj;
};

typedef struct renderer_push_constant renderer_push_constant;
struct renderer_push_constant
{
    u32 transform_id;
};

typedef struct renderer_buffers renderer_buffers;
struct renderer_buffers
{
    VkBuffer    vertex_buf;
    VkBuffer    index_buf;
    VkBuffer    stage_buf;
    VkBuffer    ubo_buf[RENDERER_FRAMES_IN_FLIGHT];
    VkBuffer    ssbo_buf[RENDERER_FRAMES_IN_FLIGHT];

    VkImage     depth_image;
    VkImageView depth_image_view;

    gpu_mem*    vertex_mem;
    gpu_mem*    index_mem;
    gpu_mem*    stage_mem;
    gpu_mem*    ubo_mem[RENDERER_FRAMES_IN_FLIGHT];
    gpu_mem*    ssbo_mem[RENDERER_FRAMES_IN_FLIGHT];
    gpu_mem*    depth_mem;

    void*       stage_mapped;
    void*       ubo_mapped[RENDERER_FRAMES_IN_FLIGHT];
    void*       ssbo_mapped[RENDERER_FRAMES_IN_FLIGHT];
};

typedef struct renderer_mesh renderer_mesh;
struct renderer_mesh
{
    u32 vertex_offset;
    u32 vertex_count;
    i32 index_offset;
    u32 index_count;
};

typedef struct renderer_pipeline renderer_pipeline;
struct renderer_pipeline
{
    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorSet       descriptor_sets[RENDERER_FRAMES_IN_FLIGHT];
    VkPipelineLayout      graphics_pipeline_layout;
    VkPipeline            graphics_pipeline;
};

typedef struct renderer renderer;
struct renderer
{
    gpu_arena          gpu_arena;
    cpu_arena*         host_arena;
    VkInstance         instance;
    VkSurfaceKHR       surface;
    VkPhysicalDevice   physical_device;
    VkDevice           device;
    renderer_queue_ids queue_ids;
    VkQueue            graphics_queue;
    VkQueue            present_queue;
    VkSwapchainKHR     swapchain;
    VkImage*           swapchain_images;
    VkImageView*       swapchain_img_views;
    VkExtent2D         swapchain_extent;
    VkFormat           swapchain_img_fmt;
    VkCommandPool      command_pool;
    VkDescriptorPool   descriptor_pool;
    VkCommandBuffer    command_buffers[RENDERER_FRAMES_IN_FLIGHT];
    VkSemaphore        sem_img_avail[RENDERER_FRAMES_IN_FLIGHT];
    VkSemaphore        sem_render_end[RENDERER_FRAMES_IN_FLIGHT];
    VkFence            fence_in_flight[RENDERER_FRAMES_IN_FLIGHT];

    renderer_buffers   buffers;

    renderer_pipeline* pipelines;
    u64                pipeline_count;

    renderer_mesh*     mesh_data;
    i32                mesh_count;
};

global renderer g_renderer = {0};

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

void renderer_init(platform_hnd window_handle);
void renderer_update(platform_hnd window_handle);
void renderer_destroy();

void renderer_pipeline_init(renderer_pipeline* state);
void renderer_pipeline_destroy(renderer_pipeline* state);

void renderer_create_instance();
void renderer_create_surface(platform_hnd window_handle);
void renderer_create_physical_device();
void renderer_create_queue_ids();
void renderer_create_device();
void renderer_create_swapchain(platform_hnd window_handle);
void renderer_create_command_pool();
void renderer_create_command_buffers();
void renderer_create_descriptor_pool();
void renderer_create_sync_primitives();
void renderer_create_resources();

void renderer_create_mesh(mesh* m);
void renderer_create_depth_resources();
void renderer_create_buffer(VkBuffer* buffer, VkDeviceSize size, VkBufferUsageFlags usage);
void renderer_create_image(VkImage* image, u32 width, u32 height, VkImageUsageFlags usage, VkImageTiling tiling, VkFormat format);
void renderer_create_image_view(VkImage image, VkImageView* view, VkFormat format, VkImageAspectFlags aspect_flags);
void renderer_copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize offset_src, VkDeviceSize offset_dst, VkDeviceSize size);

gpu_mem* renderer_create_buffer_memory(VkBuffer buffer, gpu_mem_type mem_type);
gpu_mem* renderer_create_image_memory(VkImage image, gpu_mem_type mem_type);

void           renderer_pipeline_create_descriptor_set_layout(renderer_pipeline* pipeline);
void           renderer_pipeline_create_descriptor_sets(renderer_pipeline* pipeline);
void           renderer_pipeline_create_graphics_pipeline_layout(renderer_pipeline* pipeline);
void           renderer_pipeline_create_graphics_pipeline(renderer_pipeline* pipeline);
VkShaderModule renderer_pipeline_create_shader_module(const u8* code, u64 code_size);

void               renderer_swapchain_recreate(platform_hnd window_handle);
VkSurfaceFormatKHR renderer_swapchain_find_format();
VkPresentModeKHR   renderer_swapchain_find_present();
VkExtent2D         renderer_swapchain_find_extent(platform_hnd window_handle);

void renderer_command_buffer_record(renderer_pipeline* pipeline, u32 buffer_id, u32 img_id);

b32 renderer_check_validation_layers();
b32 renderer_check_instance_extensions();
b32 renderer_check_device_extensions(VkPhysicalDevice device);
b32 renderer_device_is_suitable(VkPhysicalDevice device);

#endif // RENDERER_H 
