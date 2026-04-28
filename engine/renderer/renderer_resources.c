internal void renderer_resources_init()
{
    VkBufferUsageFlags flags_vert = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    VkBufferUsageFlags flags_idx  = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    VkBufferUsageFlags flags_stg  = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkBufferUsageFlags flags_ubo  = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    VkBufferUsageFlags flags_ssbo = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    VkBufferUsageFlags flags_dcmd = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    VkBufferUsageFlags flags_draw = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    renderer_resources_create_buffer(&g_renderer.resources.vertex_buf, GPU_MEM_SIZE_VERTEX, flags_vert);
    renderer_resources_create_buffer(&g_renderer.resources.index_buf, GPU_MEM_SIZE_INDEX, flags_idx);
    renderer_resources_create_buffer(&g_renderer.resources.stage_buf, GPU_MEM_SIZE_STG, flags_stg);

    renderer_resources_create_buffer(&g_renderer.resources.ubo_buf, GPU_MEM_SIZE_UBO, flags_ubo);
    renderer_resources_create_buffer(&g_renderer.resources.ssbo_buf, GPU_MEM_SIZE_SSBO, flags_ssbo);
    renderer_resources_create_buffer(&g_renderer.resources.dcmd_buf, GPU_MEM_SIZE_DCMD, flags_dcmd);
    renderer_resources_create_buffer(&g_renderer.resources.draw_buf, GPU_MEM_SIZE_DRAW, flags_draw);

    g_renderer.resources.vertex_mem = renderer_resources_create_buffer_memory(g_renderer.resources.vertex_buf, GPU_MEM_TYPE_mesh);
    g_renderer.resources.index_mem  = renderer_resources_create_buffer_memory(g_renderer.resources.index_buf, GPU_MEM_TYPE_mesh);
    g_renderer.resources.stage_mem  = renderer_resources_create_buffer_memory(g_renderer.resources.stage_buf, GPU_MEM_TYPE_stg);

    VkResult vk_result = vkMapMemory(
        g_renderer.device,
        g_renderer.resources.stage_mem->memory,
        g_renderer.resources.stage_mem->offset,
        GPU_MEM_SIZE_STG,
        0,
        &g_renderer.resources.stage_mapped
    );

    EMBER_ASSERT(vk_result == VK_SUCCESS);

    g_renderer.resources.ubo_mem  = renderer_resources_create_buffer_memory(g_renderer.resources.ubo_buf, GPU_MEM_TYPE_ubo);
    g_renderer.resources.ssbo_mem = renderer_resources_create_buffer_memory(g_renderer.resources.ssbo_buf, GPU_MEM_TYPE_ssbo);
    g_renderer.resources.dcmd_mem = renderer_resources_create_buffer_memory(g_renderer.resources.dcmd_buf, GPU_MEM_TYPE_dcmd);
    g_renderer.resources.draw_mem = renderer_resources_create_buffer_memory(g_renderer.resources.draw_buf, GPU_MEM_TYPE_draw);

    vk_result = vkMapMemory(
        g_renderer.device,
        g_renderer.resources.ubo_mem->memory,
        g_renderer.resources.ubo_mem->offset,
        GPU_MEM_SIZE_UBO,
        0,
        &g_renderer.resources.ubo_mapped
    );

    EMBER_ASSERT(vk_result == VK_SUCCESS);

    vk_result = vkMapMemory(
        g_renderer.device,
        g_renderer.resources.ssbo_mem->memory,
        g_renderer.resources.ssbo_mem->offset,
        GPU_MEM_SIZE_SSBO,
        0,
        &g_renderer.resources.ssbo_mapped
    );

    EMBER_ASSERT(vk_result == VK_SUCCESS);

    vk_result = vkMapMemory(
        g_renderer.device,
        g_renderer.resources.dcmd_mem->memory,
        g_renderer.resources.dcmd_mem->offset,
        GPU_MEM_SIZE_DCMD,
        0,
        &g_renderer.resources.dcmd_mapped
    );

    EMBER_ASSERT(vk_result == VK_SUCCESS);

    vk_result = vkMapMemory(
        g_renderer.device,
        g_renderer.resources.draw_mem->memory,
        g_renderer.resources.draw_mem->offset,
        GPU_MEM_SIZE_DRAW,
        0,
        &g_renderer.resources.draw_mapped
    );

    EMBER_ASSERT(vk_result == VK_SUCCESS);

    renderer_resources_create_depth();
}

internal void renderer_resources_destroy()
{
    vkUnmapMemory(g_renderer.device, g_renderer.resources.stage_mem->memory);
    vkDestroyBuffer(g_renderer.device, g_renderer.resources.stage_buf, NULL);

    vkUnmapMemory(g_renderer.device, g_renderer.resources.ubo_mem->memory);
    vkDestroyBuffer(g_renderer.device, g_renderer.resources.ubo_buf, NULL);

    vkUnmapMemory(g_renderer.device, g_renderer.resources.ssbo_mem->memory);
    vkDestroyBuffer(g_renderer.device, g_renderer.resources.ssbo_buf, NULL);

    vkUnmapMemory(g_renderer.device, g_renderer.resources.dcmd_mem->memory);
    vkDestroyBuffer(g_renderer.device, g_renderer.resources.dcmd_buf, NULL);

    vkUnmapMemory(g_renderer.device, g_renderer.resources.draw_mem->memory);
    vkDestroyBuffer(g_renderer.device, g_renderer.resources.draw_buf, NULL);

    vkDestroyImageView(g_renderer.device, g_renderer.resources.depth_image_view, NULL);
    vkDestroyImage(g_renderer.device, g_renderer.resources.depth_image, NULL);

    vkDestroyBuffer(g_renderer.device, g_renderer.resources.vertex_buf, NULL);
    vkDestroyBuffer(g_renderer.device, g_renderer.resources.index_buf, NULL);
}

internal void renderer_resources_create_depth()
{
    VkFormat formats[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    VkFormat depth_format = VK_FORMAT_UNDEFINED;

    for (u32 i = 0; i < ARRAY_COUNT(formats); i++)
    {
        VkFormatProperties format_props;
        vkGetPhysicalDeviceFormatProperties(g_renderer.physical_device, formats[i], &format_props);

        if (format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            depth_format = formats[i];
            break;
        }
    }

    EMBER_ASSERT(depth_format != VK_FORMAT_UNDEFINED);

    renderer_resources_create_image(
        &g_renderer.resources.depth_image,
        g_renderer.swapchain_extent.width,
        g_renderer.swapchain_extent.height,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_IMAGE_TILING_OPTIMAL,
        depth_format
    );

    g_renderer.resources.depth_mem = renderer_resources_create_image_memory(g_renderer.resources.depth_image, GPU_MEM_TYPE_tex);

    renderer_resources_create_image_view(
        g_renderer.resources.depth_image,
        &g_renderer.resources.depth_image_view,
        depth_format,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );

    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool                 = g_renderer.command_pool;
    alloc_info.commandBufferCount          = 1;

    VkCommandBuffer cmd_buffer;
    VkResult vk_result = vkAllocateCommandBuffers(g_renderer.device, &alloc_info, &cmd_buffer);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    VkCommandBufferBeginInfo begin_info = {0};
    begin_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd_buffer, &begin_info);

    VkImageMemoryBarrier2 image_barrier           = {0};
    image_barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    image_barrier.srcStageMask                    = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    image_barrier.srcAccessMask                   = 0;
    image_barrier.dstStageMask                    = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
    image_barrier.dstAccessMask                   = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    image_barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
    image_barrier.newLayout                       = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    image_barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    image_barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    image_barrier.image                           = g_renderer.resources.depth_image;
    image_barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
    image_barrier.subresourceRange.baseArrayLayer = 0;
    image_barrier.subresourceRange.layerCount     = 1;
    image_barrier.subresourceRange.baseMipLevel   = 0;
    image_barrier.subresourceRange.levelCount     = 1;

    VkDependencyInfo dependency_info        = {0};
    dependency_info.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency_info.dependencyFlags         = 0;
    dependency_info.imageMemoryBarrierCount = 1;
    dependency_info.pImageMemoryBarriers    = &image_barrier;

    vkCmdPipelineBarrier2(cmd_buffer, &dependency_info);

    vkEndCommandBuffer(cmd_buffer);

    VkCommandBufferSubmitInfo cmd_buffer_info = {0};
    cmd_buffer_info.sType                     = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmd_buffer_info.commandBuffer             = cmd_buffer;

    VkSubmitInfo2 submit_info          = {0};
    submit_info.sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos    = &cmd_buffer_info;

    vkQueueSubmit2(g_renderer.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(g_renderer.graphics_queue);

    vkFreeCommandBuffers(g_renderer.device, g_renderer.command_pool, 1, &cmd_buffer);
}

internal void renderer_resources_create_buffer(VkBuffer* buffer, VkDeviceSize size, VkBufferUsageFlags usage)
{
    VkBufferCreateInfo buffer_info = {0};
    buffer_info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.flags              = 0;
    buffer_info.usage              = usage;
    buffer_info.size               = size;
    buffer_info.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

    VkResult vk_result = vkCreateBuffer(g_renderer.device, &buffer_info, NULL, buffer);
    EMBER_ASSERT(vk_result == VK_SUCCESS);
}

internal void renderer_resources_create_image(VkImage* image, u32 width, u32 height, VkImageUsageFlags usage, VkImageTiling tiling, VkFormat format)
{
    VkImageCreateInfo image_info     = {0};
    image_info.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.flags                 = 0;
    image_info.imageType             = VK_IMAGE_TYPE_2D;
    image_info.format                = format;
    image_info.extent.width          = width;
    image_info.extent.height         = height;
    image_info.extent.depth          = 1;
    image_info.mipLevels             = 1;
    image_info.arrayLayers           = 1;
    image_info.samples               = VK_SAMPLE_COUNT_1_BIT;
    image_info.tiling                = tiling;
    image_info.usage                 = usage;
    image_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices   = NULL;
    image_info.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;

    VkResult vk_result = vkCreateImage(g_renderer.device, &image_info, NULL, image);
    EMBER_ASSERT(vk_result == VK_SUCCESS);
}

internal void renderer_resources_create_image_view(VkImage image, VkImageView* view, VkFormat format, VkImageAspectFlags aspect_flags)
{
    VkImageViewCreateInfo view_info           = {0};
    view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.flags                           = 0;
    view_info.image                           = image;
    view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format                          = format;
    view_info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.subresourceRange.aspectMask     = aspect_flags;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount     = 1;
    view_info.subresourceRange.baseMipLevel   = 0;
    view_info.subresourceRange.levelCount     = 1;

    VkResult vk_result = vkCreateImageView(g_renderer.device, &view_info, NULL, view);
    EMBER_ASSERT(vk_result == VK_SUCCESS);
}

internal void renderer_resources_copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize offset_src, VkDeviceSize offset_dst, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool                 = g_renderer.command_pool;
    alloc_info.commandBufferCount          = 1;

    VkCommandBuffer cmd_buffer;
    VkResult vk_result = vkAllocateCommandBuffers(g_renderer.device, &alloc_info, &cmd_buffer);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    VkCommandBufferBeginInfo begin_info = {0};
    begin_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd_buffer, &begin_info);

    VkBufferCopy copy_region = {0};
    copy_region.srcOffset    = offset_src;
    copy_region.dstOffset    = offset_dst;
    copy_region.size         = size;

    vkCmdCopyBuffer(cmd_buffer, src, dst, 1, &copy_region);

    vkEndCommandBuffer(cmd_buffer);

    VkCommandBufferSubmitInfo cmd_buffer_info = {0};
    cmd_buffer_info.sType                     = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmd_buffer_info.commandBuffer             = cmd_buffer;

    VkSubmitInfo2 submit_info          = {0};
    submit_info.sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos    = &cmd_buffer_info;

    vkQueueSubmit2(g_renderer.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(g_renderer.graphics_queue);

    vkFreeCommandBuffers(g_renderer.device, g_renderer.command_pool, 1, &cmd_buffer);
}

internal gpu_mem_t* renderer_resources_create_buffer_memory(VkBuffer buffer, gpu_mem_type_t mem_type)
{
    VkMemoryRequirements mem_req;
    vkGetBufferMemoryRequirements(g_renderer.device, buffer, &mem_req);

    EMBER_ASSERT((mem_req.memoryTypeBits & (1 << g_renderer.gpu_arena.blocks[mem_type].mem_idx)) != 0);

    gpu_mem_t* alloc = gpu_arena_alloc(&g_renderer.gpu_arena, mem_req.size, mem_req.alignment, mem_type);

    vkBindBufferMemory(g_renderer.device, buffer, alloc->memory, alloc->offset);

    return alloc;
}

internal gpu_mem_t* renderer_resources_create_image_memory(VkImage image, gpu_mem_type_t mem_type)
{
    VkMemoryRequirements mem_req;
    vkGetImageMemoryRequirements(g_renderer.device, image, &mem_req);

    EMBER_ASSERT((mem_req.memoryTypeBits & (1 << g_renderer.gpu_arena.blocks[mem_type].mem_idx)) != 0);

    gpu_mem_t* alloc = gpu_arena_alloc(&g_renderer.gpu_arena, mem_req.size, mem_req.alignment, mem_type);

    vkBindImageMemory(g_renderer.device, image, alloc->memory, alloc->offset);

    return alloc;
}
