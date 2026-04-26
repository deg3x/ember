#ifndef RENDERER_RESOURCES_H
#define RENDERER_RESOURCES_H

typedef struct renderer_resources_t renderer_resources_t;
struct renderer_resources_t
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

internal void renderer_resources_init();
internal void renderer_resources_destroy();

internal void renderer_resources_create_depth();
internal void renderer_resources_create_buffer(VkBuffer* buffer, VkDeviceSize size, VkBufferUsageFlags usage);
internal void renderer_resources_create_image(VkImage* image, u32 width, u32 height, VkImageUsageFlags usage, VkImageTiling tiling, VkFormat format);
internal void renderer_resources_create_image_view(VkImage image, VkImageView* view, VkFormat format, VkImageAspectFlags aspect_flags);
internal void renderer_resources_copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize offset_src, VkDeviceSize offset_dst, VkDeviceSize size);

internal gpu_mem_t* renderer_resources_create_buffer_memory(VkBuffer buffer, gpu_mem_type_t mem_type);
internal gpu_mem_t* renderer_resources_create_image_memory(VkImage image, gpu_mem_type_t mem_type);


#endif // RENDERER_RESOURCES_H
