internal gpu_arena_t
gpu_arena_init(VkPhysicalDevice physical_device, VkDevice device)
{
    gpu_arena_t ret = {0};

    vkGetPhysicalDeviceMemoryProperties(physical_device, &ret.mem_props);

    u32_t mem_idx_staging  = U32_MAX;
    u32_t mem_idx_device   = U32_MAX;
    u32_t mem_idx_host_vco = U32_MAX;

    VkMemoryPropertyFlags flags_staging  = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VkMemoryPropertyFlags flags_device   = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkMemoryPropertyFlags flags_host_vco = flags_staging;

    for (u32_t i = 0; i < ret.mem_props.memoryTypeCount; i++)
    {
        b32_t flag_check_staging  = (ret.mem_props.memoryTypes[i].propertyFlags & flags_staging) == flags_staging;
        b32_t flag_check_device   = (ret.mem_props.memoryTypes[i].propertyFlags & flags_device) == flags_device;
        b32_t flag_check_host_vco = (ret.mem_props.memoryTypes[i].propertyFlags & flags_host_vco) == flags_host_vco;

        if (flag_check_staging && mem_idx_staging == U32_MAX)
        {
            mem_idx_staging = i;
        }

        if (flag_check_device && mem_idx_device == U32_MAX)
        {
            mem_idx_device = i;
        }

        if (flag_check_host_vco && mem_idx_host_vco == U32_MAX)
        {
            mem_idx_host_vco = i;
        }

        if (mem_idx_staging != U32_MAX && mem_idx_device != U32_MAX && mem_idx_host_vco != U32_MAX)
        {
            break;
        }
    }

    EMBER_ASSERT(mem_idx_staging != U32_MAX);
    EMBER_ASSERT(mem_idx_device != U32_MAX);
    EMBER_ASSERT(mem_idx_host_vco != U32_MAX);

    VkMemoryAllocateInfo alloc_info_staging = {0};
    alloc_info_staging.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info_staging.allocationSize       = GPU_MEM_SIZE_STAGING;
    alloc_info_staging.memoryTypeIndex      = mem_idx_staging;

    VkMemoryAllocateInfo alloc_info_device = {0};
    alloc_info_device.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info_device.allocationSize       = GPU_MEM_SIZE_DEVICE;
    alloc_info_device.memoryTypeIndex      = mem_idx_device;

    VkMemoryAllocateInfo alloc_info_host_vco = {0};
    alloc_info_host_vco.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info_host_vco.allocationSize       = GPU_MEM_SIZE_HOST_VCO;
    alloc_info_host_vco.memoryTypeIndex      = mem_idx_host_vco;

    VkResult vk_result;

    vk_result = vkAllocateMemory(device, &alloc_info_staging, NULL, &ret.blocks[GPU_MEM_TYPE_staging].memory);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    vk_result = vkAllocateMemory(device, &alloc_info_device, NULL, &ret.blocks[GPU_MEM_TYPE_device].memory);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    vk_result = vkAllocateMemory(device, &alloc_info_host_vco, NULL, &ret.blocks[GPU_MEM_TYPE_host_vco].memory);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    ret.blocks[GPU_MEM_TYPE_staging].position  = 0;
    ret.blocks[GPU_MEM_TYPE_device].position   = 0;
    ret.blocks[GPU_MEM_TYPE_host_vco].position = 0;
    ret.blocks[GPU_MEM_TYPE_staging].size      = GPU_MEM_SIZE_STAGING;
    ret.blocks[GPU_MEM_TYPE_device].size       = GPU_MEM_SIZE_DEVICE;
    ret.blocks[GPU_MEM_TYPE_host_vco].size     = GPU_MEM_SIZE_HOST_VCO;
    ret.blocks[GPU_MEM_TYPE_staging].mem_idx   = mem_idx_staging;
    ret.blocks[GPU_MEM_TYPE_device].mem_idx    = mem_idx_device;
    ret.blocks[GPU_MEM_TYPE_host_vco].mem_idx  = mem_idx_host_vco;

    arena_params_t host_arena_params = { KB(32), KB(32), 0 };

    ret.host_arena = arena_init(&host_arena_params);

    return ret;
}

internal gpu_mem_t*
gpu_arena_alloc(gpu_arena_t* arena, u64_t size, u64_t alignment, gpu_mem_type_t type)
{
    EMBER_ASSERT(arena != NULL);

    u64_t alloc_size = size; //(size + alignment - 1) & !(alignment - 1);

    VkDeviceSize remaining = arena->blocks[type].size - arena->blocks[type].position;

    EMBER_ASSERT(alloc_size <= remaining);

    gpu_mem_t* ret = MEMORY_PUSH(arena->host_arena, gpu_mem_t, 1);

    arena->blocks[type].position += alloc_size;

    ret->memory = arena->blocks[type].memory;
    ret->offset = 0;
    ret->size   = alloc_size;
    ret->type   = type;
    ret->next   = NULL;

    // TODO(KB): Traverse the free list first

    gpu_mem_t* prev = arena->entries[type];
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

internal void
gpu_arena_free(gpu_arena_t* arena, gpu_mem_t* memory)
{
    EMBER_ASSERT(arena != NULL);
    EMBER_ASSERT(memory != NULL);

    gpu_mem_t* prev = arena->entries[memory->type];
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

    gpu_mem_t* last_free = arena->free_list[memory->type];
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

internal void
gpu_arena_release(gpu_arena_t* arena, VkDevice device)
{
    EMBER_ASSERT(arena != NULL);

    for (u32_t i = 0; i < GPU_MEM_TYPE_count; i++)
    {
        vkFreeMemory(device, arena->blocks[i].memory, NULL);
    }

    for (u32_t i = 0; i < GPU_MEM_TYPE_count; i++)
    {
        arena->entries[i] = NULL;
        arena->blocks[i]  = (gpu_mem_block_t){0};
    }

    arena_release(arena->host_arena);
}
