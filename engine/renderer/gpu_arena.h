#ifndef GPU_ARENA_H
#define GPU_ARENA_H

#define GPU_MEM_SIZE_STAGING  MB(64)
#define GPU_MEM_SIZE_DEVICE   MB(1024)
#define GPU_MEM_SIZE_HOST_VCO MB(64)

#define GPU_MEM_SIZE_BUF_VERTEX (GPU_MEM_SIZE_DEVICE / 4)
#define GPU_MEM_SIZE_BUF_INDEX  (GPU_MEM_SIZE_DEVICE / 4)

typedef u32_t gpu_mem_type_t;
enum
{
    GPU_MEM_TYPE_staging  = 0,
    GPU_MEM_TYPE_device   = 1,
    GPU_MEM_TYPE_host_vco = 2,
    GPU_MEM_TYPE_count
};

typedef struct gpu_mem_block_t gpu_mem_block_t;
struct gpu_mem_block_t
{
    VkDeviceMemory memory;
    VkDeviceSize   size;
    VkDeviceSize   position;
    u32_t          mem_idx;
};

typedef struct gpu_mem_t gpu_mem_t;
struct gpu_mem_t
{
    VkDeviceMemory memory;
    u64_t          offset;
    u64_t          size;

    gpu_mem_t*     next;
    gpu_mem_type_t type;
};

typedef struct gpu_arena_t gpu_arena_t;
struct gpu_arena_t
{
    arena_t*        host_arena;
    gpu_mem_t*      entries[GPU_MEM_TYPE_count];
    gpu_mem_t*      free_list[GPU_MEM_TYPE_count];
    gpu_mem_block_t blocks[GPU_MEM_TYPE_count];

    VkPhysicalDeviceMemoryProperties mem_props;
};

internal gpu_arena_t gpu_arena_init(VkPhysicalDevice physical_device, VkDevice device);
internal gpu_mem_t*  gpu_arena_alloc(gpu_arena_t* arena, u64_t size, u64_t alignment, gpu_mem_type_t type);
internal void        gpu_arena_free(gpu_arena_t* arena, gpu_mem_t* memory);
internal void        gpu_arena_release(gpu_arena_t* arena, VkDevice device);

#endif //GPU_ARENA_H
