#ifndef GPU_ARENA_H
#define GPU_ARENA_H

#define GPU_MEM_SIZE_MESH MB(512)
#define GPU_MEM_SIZE_TEX  MB(512)
#define GPU_MEM_SIZE_STG  MB(32)
#define GPU_MEM_SIZE_UBO  KB(8)
#define GPU_MEM_SIZE_SSBO MB(32)
#define GPU_MEM_SIZE_DCMD MB(8)
#define GPU_MEM_SIZE_DRAW MB(8)

#define GPU_MEM_SIZE_VERTEX (GPU_MEM_SIZE_MESH / 2)
#define GPU_MEM_SIZE_INDEX  (GPU_MEM_SIZE_MESH / 2)

typedef u32 gpu_mem_type_t;
enum
{
    GPU_MEM_TYPE_mesh,
    GPU_MEM_TYPE_tex,
    GPU_MEM_TYPE_stg,
    GPU_MEM_TYPE_ubo,
    GPU_MEM_TYPE_ssbo,
    GPU_MEM_TYPE_dcmd,
    GPU_MEM_TYPE_draw,
    GPU_MEM_TYPE_count
};

typedef struct gpu_mem_block_t gpu_mem_block_t;
struct gpu_mem_block_t
{
    VkDeviceMemory memory;
    VkDeviceSize   size;
    VkDeviceSize   position;
    u32            mem_idx;
};

typedef struct gpu_mem_t gpu_mem_t;
struct gpu_mem_t
{
    VkDeviceMemory memory;
    u64            offset;
    u64            size;

    gpu_mem_t*     next;
    gpu_mem_type_t type;
};

typedef struct gpu_arena_t gpu_arena_t;
struct gpu_arena_t
{
    cpu_arena_t*    host_arena;
    gpu_mem_t*      entries[GPU_MEM_TYPE_count];
    gpu_mem_t*      free_list[GPU_MEM_TYPE_count];
    gpu_mem_block_t blocks[GPU_MEM_TYPE_count];

    VkPhysicalDeviceMemoryProperties mem_props;
};

internal gpu_arena_t gpu_arena_init(VkPhysicalDevice physical_device, VkDevice device);
internal gpu_mem_t*  gpu_arena_alloc(gpu_arena_t* arena, u64 size, u64 alignment, gpu_mem_type_t type);
internal void        gpu_arena_free(gpu_arena_t* arena, gpu_mem_t* memory);
internal void        gpu_arena_release(gpu_arena_t* arena, VkDevice device);

#endif //GPU_ARENA_H
