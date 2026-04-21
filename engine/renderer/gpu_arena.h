#ifndef GPU_ARENA_H
#define GPU_ARENA_H

#define GPU_MEM_SIZE_MESH MB(512)
#define GPU_MEM_SIZE_TEX  MB(512)
#define GPU_MEM_SIZE_STG  MB(32)
#define GPU_MEM_SIZE_UBO  KB(4)
#define GPU_MEM_SIZE_SSBO MB(32)

#define GPU_MEM_SIZE_VERTEX (GPU_MEM_SIZE_MESH / 2)
#define GPU_MEM_SIZE_INDEX  (GPU_MEM_SIZE_MESH / 2)

typedef u32 gpu_mem_type;
enum
{
    GPU_MEM_TYPE_mesh,
    GPU_MEM_TYPE_tex,
    GPU_MEM_TYPE_stg,
    GPU_MEM_TYPE_ubo,
    GPU_MEM_TYPE_ssbo,
    GPU_MEM_TYPE_count
};

typedef struct gpu_mem_block gpu_mem_block;
struct gpu_mem_block
{
    VkDeviceMemory memory;
    VkDeviceSize   size;
    VkDeviceSize   position;
    u32            mem_idx;
};

typedef struct gpu_mem gpu_mem;
struct gpu_mem
{
    VkDeviceMemory memory;
    u64            offset;
    u64            size;

    gpu_mem*       next;
    gpu_mem_type   type;
};

typedef struct gpu_arena gpu_arena;
struct gpu_arena
{
    cpu_arena*    host_arena;
    gpu_mem*      entries[GPU_MEM_TYPE_count];
    gpu_mem*      free_list[GPU_MEM_TYPE_count];
    gpu_mem_block blocks[GPU_MEM_TYPE_count];

    VkPhysicalDeviceMemoryProperties mem_props;
};

internal gpu_arena gpu_arena_init(VkPhysicalDevice physical_device, VkDevice device);
internal gpu_mem*  gpu_arena_alloc(gpu_arena* arena, u64 size, u64 alignment, gpu_mem_type type);
internal void      gpu_arena_free(gpu_arena* arena, gpu_mem* memory);
internal void      gpu_arena_release(gpu_arena* arena, VkDevice device);

#endif //GPU_ARENA_H
