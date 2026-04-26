#ifndef RENDERER_H 
#define RENDERER_H 

typedef struct renderer_queue_ids_t renderer_queue_ids_t;
struct renderer_queue_ids_t
{
    u32 graphics;
    u32 presentation;
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

    renderer_resources_t resources;

    renderer_pipeline_t* pipelines;
    renderer_mesh_t*     mesh_data;
    renderer_node_t*     node_data;

    i32                  pipeline_count;
    i32                  mesh_count;
    i32                  node_count;
};

global renderer_t g_renderer = {0};

internal void renderer_init(platform_hnd_t window_handle);
internal void renderer_update(platform_hnd_t window_handle);
internal void renderer_destroy();

internal void renderer_create_instance();
internal void renderer_create_surface(platform_hnd_t window_handle);
internal void renderer_create_physical_device();
internal void renderer_create_queue_ids();
internal void renderer_create_device();
internal void renderer_create_command_pool();
internal void renderer_create_command_buffers();
internal void renderer_create_descriptor_pool();
internal void renderer_create_sync_primitives();

internal void renderer_create_nodes(renderer_node_t* nodes, renderer_ssbo_t* node_ssbo, i32 node_count);
internal void renderer_create_meshes(mesh_t* m, i32 count);

internal void renderer_command_buffer_record(renderer_pipeline_t* pipeline, u32 buffer_id, u32 img_id);

internal b32 renderer_check_validation_layers();
internal b32 renderer_check_instance_extensions();
internal b32 renderer_check_device_extensions(VkPhysicalDevice device);
internal b32 renderer_device_is_suitable(VkPhysicalDevice device);

#endif // RENDERER_H 
