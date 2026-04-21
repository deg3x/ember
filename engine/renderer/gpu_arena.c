internal gpu_arena gpu_arena_init(VkPhysicalDevice physical_device, VkDevice device)
{
    gpu_arena ret = {0};

    vkGetPhysicalDeviceMemoryProperties(physical_device, &ret.mem_props);

    u32 mem_idx_mesh = U32_MAX;
    u32 mem_idx_tex  = U32_MAX;
    u32 mem_idx_stg  = U32_MAX;
    u32 mem_idx_ssbo = U32_MAX;
    u32 mem_idx_ubo  = U32_MAX;

    VkMemoryPropertyFlags flags_mesh = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkMemoryPropertyFlags flags_tex  = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkMemoryPropertyFlags flags_stg  = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VkMemoryPropertyFlags flags_ubo  = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VkMemoryPropertyFlags flags_ssbo = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    for (u32 i = 0; i < ret.mem_props.memoryTypeCount; i++)
    {
        b32 flag_check_mesh = (ret.mem_props.memoryTypes[i].propertyFlags & flags_mesh) == flags_mesh;
        b32 flag_check_tex  = (ret.mem_props.memoryTypes[i].propertyFlags & flags_tex) == flags_tex;
        b32 flag_check_stg  = (ret.mem_props.memoryTypes[i].propertyFlags & flags_stg) == flags_stg;
        b32 flag_check_ubo  = (ret.mem_props.memoryTypes[i].propertyFlags & flags_ubo) == flags_ubo;
        b32 flag_check_ssbo = (ret.mem_props.memoryTypes[i].propertyFlags & flags_ssbo) == flags_ssbo;

        if (flag_check_mesh && mem_idx_mesh == U32_MAX)
        {
            mem_idx_mesh = i;
        }

        if (flag_check_tex && mem_idx_tex == U32_MAX)
        {
            mem_idx_tex = i;
        }

        if (flag_check_stg && mem_idx_stg == U32_MAX)
        {
            mem_idx_stg = i;
        }

        if (flag_check_ubo && mem_idx_ubo == U32_MAX)
        {
            mem_idx_ubo = i;
        }

        if (flag_check_ssbo && mem_idx_ssbo == U32_MAX)
        {
            mem_idx_ssbo = i;
        }

        if (mem_idx_mesh != U32_MAX && mem_idx_tex != U32_MAX && mem_idx_stg != U32_MAX && mem_idx_ubo != U32_MAX)
        {
            break;
        }
    }

    EMBER_ASSERT(mem_idx_mesh != U32_MAX);
    EMBER_ASSERT(mem_idx_tex != U32_MAX);
    EMBER_ASSERT(mem_idx_stg != U32_MAX);
    EMBER_ASSERT(mem_idx_ubo != U32_MAX);
    EMBER_ASSERT(mem_idx_ssbo != U32_MAX);

    VkMemoryAllocateInfo alloc_info_mesh = {0};
    alloc_info_mesh.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info_mesh.allocationSize       = GPU_MEM_SIZE_MESH;
    alloc_info_mesh.memoryTypeIndex      = mem_idx_mesh;

    VkMemoryAllocateInfo alloc_info_tex = {0};
    alloc_info_tex.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info_tex.allocationSize       = GPU_MEM_SIZE_TEX;
    alloc_info_tex.memoryTypeIndex      = mem_idx_mesh;

    VkMemoryAllocateInfo alloc_info_stg = {0};
    alloc_info_stg.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info_stg.allocationSize       = GPU_MEM_SIZE_STG;
    alloc_info_stg.memoryTypeIndex      = mem_idx_stg;

    VkMemoryAllocateInfo alloc_info_ubo = {0};
    alloc_info_ubo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info_ubo.allocationSize       = GPU_MEM_SIZE_UBO;
    alloc_info_ubo.memoryTypeIndex      = mem_idx_ubo;

    VkMemoryAllocateInfo alloc_info_ssbo = {0};
    alloc_info_ssbo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info_ssbo.allocationSize       = GPU_MEM_SIZE_SSBO;
    alloc_info_ssbo.memoryTypeIndex      = mem_idx_ssbo;

    VkResult vk_result;

    vk_result = vkAllocateMemory(device, &alloc_info_mesh, NULL, &ret.blocks[GPU_MEM_TYPE_mesh].memory);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    vk_result = vkAllocateMemory(device, &alloc_info_tex, NULL, &ret.blocks[GPU_MEM_TYPE_tex].memory);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    vk_result = vkAllocateMemory(device, &alloc_info_stg, NULL, &ret.blocks[GPU_MEM_TYPE_stg].memory);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    vk_result = vkAllocateMemory(device, &alloc_info_ubo, NULL, &ret.blocks[GPU_MEM_TYPE_ubo].memory);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    vk_result = vkAllocateMemory(device, &alloc_info_ssbo, NULL, &ret.blocks[GPU_MEM_TYPE_ssbo].memory);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    ret.blocks[GPU_MEM_TYPE_mesh].position = 0;
    ret.blocks[GPU_MEM_TYPE_tex].position  = 0;
    ret.blocks[GPU_MEM_TYPE_stg].position  = 0;
    ret.blocks[GPU_MEM_TYPE_ubo].position  = 0;
    ret.blocks[GPU_MEM_TYPE_ssbo].position = 0;
    ret.blocks[GPU_MEM_TYPE_mesh].size     = GPU_MEM_SIZE_MESH;
    ret.blocks[GPU_MEM_TYPE_tex].size      = GPU_MEM_SIZE_TEX;
    ret.blocks[GPU_MEM_TYPE_stg].size      = GPU_MEM_SIZE_STG;
    ret.blocks[GPU_MEM_TYPE_ubo].size      = GPU_MEM_SIZE_UBO;
    ret.blocks[GPU_MEM_TYPE_ssbo].size     = GPU_MEM_SIZE_SSBO;
    ret.blocks[GPU_MEM_TYPE_mesh].mem_idx  = mem_idx_mesh;
    ret.blocks[GPU_MEM_TYPE_tex].mem_idx   = mem_idx_tex;
    ret.blocks[GPU_MEM_TYPE_stg].mem_idx   = mem_idx_stg;
    ret.blocks[GPU_MEM_TYPE_ubo].mem_idx   = mem_idx_ubo;
    ret.blocks[GPU_MEM_TYPE_ssbo].mem_idx  = mem_idx_ssbo;

    cpu_arena_params host_arena_params = { KB(32), KB(32), 0 };

    ret.host_arena = cpu_arena_init(&host_arena_params);

    return ret;
}

internal gpu_mem* gpu_arena_alloc(gpu_arena* arena, u64 size, u64 alignment, gpu_mem_type type)
{
    EMBER_ASSERT(arena != NULL);

    u64 alloc_size = size; //(size + alignment - 1) & !(alignment - 1);

    VkDeviceSize remaining = arena->blocks[type].size - arena->blocks[type].position;

    EMBER_ASSERT(alloc_size <= remaining);

    gpu_mem* ret = MEMORY_PUSH(arena->host_arena, gpu_mem, 1);

    arena->blocks[type].position += alloc_size;

    ret->memory = arena->blocks[type].memory;
    ret->offset = 0;
    ret->size   = alloc_size;
    ret->type   = type;
    ret->next   = NULL;

    // TODO(KB): Traverse the free list first

    gpu_mem* prev = arena->entries[type];
    if (prev == NULL)
    {
        arena->entries[type] = ret;

        return ret;
    }

    while (prev->next != NULL)
    {
        prev = prev->next;
    }

    ret->offset = (prev->offset + prev->size + alignment - 1) & ~(alignment - 1);

    arena->blocks[type].position = ret->offset + alloc_size;

    prev->next = ret;

    return ret;
}

internal void gpu_arena_free(gpu_arena* arena, gpu_mem* memory)
{
    EMBER_ASSERT(arena != NULL);
    EMBER_ASSERT(memory != NULL);

    gpu_mem* prev = arena->entries[memory->type];
    if (prev == memory)
    {
        arena->blocks[memory->type].position = 0;
        arena->entries[memory->type]         = NULL;

        // NOTE(KB): The host allocator has a leak at this point.
        //           Fix when we add free lists on host memory.
        //           This also breaks for a non-stack allocator.
    }
    else
    {
        while(prev->next != NULL && prev->next != memory)
        {
            prev = prev->next;
        }

        if (prev->next == NULL)
        {
            // NOTE(KB): Memory allocation entry was not found
            EMBER_ASSERT(EMBER_FALSE);

            return;
        }

        // NOTE(KB): This ends up leaving a hole in memory since
        //           we don't touch arena->blocks[].position

        prev->next = memory->next;

        memory->next = NULL;
    }

    gpu_mem* last_free = arena->free_list[memory->type];
    if (last_free == NULL)
    {
        arena->free_list[memory->type] = memory;

        return;
    }

    while (last_free->next != NULL)
    {
        last_free = last_free->next;
    }

    last_free->next = memory;
}

internal void gpu_arena_release(gpu_arena* arena, VkDevice device)
{
    EMBER_ASSERT(arena != NULL);

    for (u32 i = 0; i < GPU_MEM_TYPE_count; i++)
    {
        vkFreeMemory(device, arena->blocks[i].memory, NULL);
    }

    for (u32 i = 0; i < GPU_MEM_TYPE_count; i++)
    {
        arena->entries[i] = NULL;
        arena->blocks[i]  = (gpu_mem_block){0};
    }

    cpu_arena_release(arena->host_arena);
}
