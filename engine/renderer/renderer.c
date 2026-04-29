internal void renderer_init(platform_hnd_t window_handle)
{
    cpu_arena_params_t params = { GB(1), GB(1), 0 };

    g_renderer.host_arena = cpu_arena_init(&params);

    renderer_create_instance();
    renderer_create_surface(window_handle);
    renderer_create_physical_device();
    renderer_create_queue_ids();
    renderer_create_device();

    g_renderer.gpu_arena = gpu_arena_init(g_renderer.physical_device, g_renderer.device);

    renderer_swapchain_init(window_handle);
    renderer_create_command_pool();
    renderer_create_command_buffers();
    renderer_create_descriptor_pool();
    renderer_create_sync_primitives();
    renderer_resources_init();

    g_renderer.mesh_data = MEMORY_PUSH_ZERO(g_renderer.host_arena, renderer_mesh_t, RENDERER_MESH_COUNT_MAX);
    g_renderer.node_data = MEMORY_PUSH_ZERO(g_renderer.host_arena, renderer_node_t, RENDERER_NODE_COUNT_MAX);

    g_renderer.pipelines      = MEMORY_PUSH_ZERO(g_renderer.host_arena, renderer_pipeline_t, 1);
    g_renderer.pipeline_count = 1;

    renderer_pipeline_init(g_renderer.pipelines);
}

internal void renderer_update(platform_hnd_t window_handle)
{
    persist u32 frame_id = 0;

    vkWaitForFences(g_renderer.device, 1, &g_renderer.fence_in_flight[frame_id], VK_TRUE, UINT64_MAX);

    u32 img_id;
    VkResult vk_result = vkAcquireNextImageKHR(
        g_renderer.device,
        g_renderer.swapchain,
        UINT64_MAX,
        g_renderer.sem_img_avail[frame_id],
        VK_NULL_HANDLE,
        &img_id
    );

    if (vk_result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        renderer_swapchain_recreate(window_handle);

        return;
    }
    else
    {
        EMBER_ASSERT(vk_result == VK_SUCCESS || vk_result == VK_SUBOPTIMAL_KHR);
    }

    vkResetFences(g_renderer.device, 1, &g_renderer.fence_in_flight[frame_id]);
    vkResetCommandBuffer(g_renderer.command_buffers[frame_id], 0);

    renderer_command_buffer_record(&g_renderer.pipelines[0], frame_id, img_id);

    renderer_ubo_t ubo;

    f32 time          = (f32)platform_timer_since_start(g_program_state.timer);
    vec3_t camera_pos = {0.0f, 1.5f, 2.0f};
    camera_pos        = vec3_rotate_axis(&camera_pos, &VEC3_UP, time * 0.3f);

    ubo.view = mat4_look_at(
        &camera_pos,
        &(vec3_t){0.0f, 0.0f, 0.0f},
        &(vec3_t){0.0f, 1.0f, 0.0f}
    );

    platform_wnd_size_t client_size = platform_gfx_wnd_client_get_size(window_handle);

    f32 aspect = (f32)client_size.width / (f32)client_size.height;

    ubo.proj = mat4_persp(30.0f, aspect, 0.01f, 1000.0f);

    void* dest = (u8 *)g_renderer.resources.ubo_mapped + frame_id * FRAME_SIZE(GPU_MEM_SIZE_UBO);
    memcpy(dest, &ubo, sizeof(ubo));

    VkSemaphoreSubmitInfo wait_sem_info = {0};
    wait_sem_info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    wait_sem_info.semaphore             = g_renderer.sem_img_avail[frame_id];
    wait_sem_info.value                 = 0;
    wait_sem_info.stageMask             = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    wait_sem_info.deviceIndex           = 0;

    VkSemaphoreSubmitInfo sgnl_sem_info = {0};
    sgnl_sem_info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    sgnl_sem_info.semaphore             = g_renderer.sem_render_end[frame_id];
    sgnl_sem_info.value                 = 0;
    sgnl_sem_info.stageMask             = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
    sgnl_sem_info.deviceIndex           = 0;

    VkCommandBufferSubmitInfo cmd_submit_info = {0};
    cmd_submit_info.sType                     = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmd_submit_info.commandBuffer             = g_renderer.command_buffers[frame_id];
    cmd_submit_info.deviceMask                = 0;

    VkSubmitInfo2 submit_info            = {0};
    submit_info.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.waitSemaphoreInfoCount   = 1;
    submit_info.pWaitSemaphoreInfos      = &wait_sem_info;
    submit_info.commandBufferInfoCount   = 1;
    submit_info.pCommandBufferInfos      = &cmd_submit_info;
    submit_info.signalSemaphoreInfoCount = 1;
    submit_info.pSignalSemaphoreInfos    = &sgnl_sem_info;

    vk_result = vkQueueSubmit2(g_renderer.graphics_queue, 1, &submit_info, g_renderer.fence_in_flight[frame_id]);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    VkPresentInfoKHR present_info   = {0};
    present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores    = &g_renderer.sem_render_end[frame_id];
    present_info.swapchainCount     = 1;
    present_info.pSwapchains        = &g_renderer.swapchain;
    present_info.pImageIndices      = &img_id;
    present_info.pResults           = NULL;

    vk_result = vkQueuePresentKHR(g_renderer.present_queue, &present_info);

    if (vk_result == VK_ERROR_OUT_OF_DATE_KHR || vk_result == VK_SUBOPTIMAL_KHR || g_window_state.is_resizing)
    {
        renderer_swapchain_recreate(window_handle);
    }
    else
    {
        EMBER_ASSERT(vk_result == VK_SUCCESS);
    }

    frame_id = (frame_id + 1) % RENDERER_FRAMES_IN_FLIGHT;
}

internal void renderer_destroy()
{
    vkDeviceWaitIdle(g_renderer.device);

    for (u32 i = 0; i < RENDERER_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(g_renderer.device, g_renderer.sem_img_avail[i], NULL);
        vkDestroySemaphore(g_renderer.device, g_renderer.sem_render_end[i], NULL);
        vkDestroyFence(g_renderer.device, g_renderer.fence_in_flight[i], NULL);
    }

    renderer_resources_destroy();

    for (i32 i = 0; i < g_renderer.pipeline_count; i++)
    {
        renderer_pipeline_destroy(g_renderer.pipelines + i);
    }

    vkDestroyDescriptorPool(g_renderer.device, g_renderer.descriptor_pool, NULL);
    vkDestroyCommandPool(g_renderer.device, g_renderer.command_pool, NULL);

    renderer_swapchain_destroy();

    gpu_arena_release(&g_renderer.gpu_arena, g_renderer.device);

    vkDestroyDevice(g_renderer.device, NULL);
    vkDestroySurfaceKHR(g_renderer.instance, g_renderer.surface, NULL);
    vkDestroyInstance(g_renderer.instance, NULL);

    cpu_arena_release(g_renderer.host_arena);
}

internal void renderer_create_instance()
{
    u32 vk_api_version = VK_API_VERSION_1_4;

    // TODO(KB): Provide appropriate names/versions for app/engine
    VkApplicationInfo app_info  = {0};
    app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext              = NULL;
    app_info.pApplicationName   = NULL;
    app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.pEngineName        = "Ember Engine";
    app_info.engineVersion      = VK_MAKE_VERSION(0, 0, 1);
    app_info.apiVersion         = vk_api_version;

    PFN_vkEnumerateInstanceVersion vk_api_version_func =
        (PFN_vkEnumerateInstanceVersion) vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");

    // TODO(KB): We probably want to notify the user of unsupported/unavailable SDK version
    EMBER_ASSERT(vk_api_version_func);
    u32 vk_api_version_supported = VK_API_VERSION_1_0;
    vk_api_version_func(&vk_api_version_supported);

    EMBER_ASSERT(vk_api_version_supported >= vk_api_version);

    b32 exts_found = renderer_check_instance_extensions();
    EMBER_ASSERT(exts_found);

#if RHI_VK_VALIDATIONS_ENABLED
    b32 layers_found = renderer_check_validation_layers();
#endif

    VkInstanceCreateInfo instance_info    = {0};
    instance_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo        = &app_info;
    instance_info.enabledExtensionCount   = ARRAY_COUNT(g_instance_extensions);
    instance_info.ppEnabledExtensionNames = g_instance_extensions;

#if RHI_VK_VALIDATIONS_ENABLED
    if (layers_found)
    {
        instance_info.enabledLayerCount   = ARRAY_COUNT(g_validation_layers);
        instance_info.ppEnabledLayerNames = g_validation_layers;
    }
    else
    {
        instance_info.enabledLayerCount = 0;
    }
#endif

    VkResult create_result = vkCreateInstance(&instance_info, NULL, &g_renderer.instance);
    EMBER_ASSERT(create_result == VK_SUCCESS);
}

internal void renderer_create_surface(platform_hnd_t window_handle)
{
#if PLATFORM_WINDOWS
    VkWin32SurfaceCreateInfoKHR surface_info = {0};
    surface_info.sType                       = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_info.hinstance                   = platform_get_instance_handle().hnd;
    surface_info.hwnd                        = window_handle.hnd;

    VkResult create_result = vkCreateWin32SurfaceKHR(g_renderer.instance, &surface_info, NULL, &g_renderer.surface);
    EMBER_ASSERT(create_result == VK_SUCCESS);
#else
    #error "Platform renderer surface not supported"
#endif
}

internal void renderer_create_physical_device()
{
    g_renderer.physical_device = VK_NULL_HANDLE;

    cpu_scratch_t scratch = cpu_scratch_begin(g_renderer.host_arena);

    u32 device_count = 0;
    vkEnumeratePhysicalDevices(g_renderer.instance, &device_count, NULL);

    EMBER_ASSERT(device_count > 0);

    VkPhysicalDevice* devices = MEMORY_PUSH(scratch.arena, VkPhysicalDevice, device_count);
    vkEnumeratePhysicalDevices(g_renderer.instance, &device_count, devices);

    for (u32 i = 0; i < device_count; i++)
    {
        if (renderer_device_is_suitable(devices[i]))
        {
            g_renderer.physical_device = devices[i];
            break;
        }
    }

    EMBER_ASSERT(g_renderer.physical_device != VK_NULL_HANDLE);

    cpu_scratch_end(scratch);
}

internal void renderer_create_queue_ids()
{
    cpu_scratch_t scratch = cpu_scratch_begin(g_renderer.host_arena);

    u32 family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(g_renderer.physical_device, &family_count, NULL);

    VkQueueFamilyProperties* family_props = MEMORY_PUSH(scratch.arena, VkQueueFamilyProperties, family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(g_renderer.physical_device, &family_count, family_props);

    b32 graphics_found = EMBER_FALSE;
    b32 present_found  = EMBER_FALSE;
    u32 graphics_id    = 0;
    u32 present_id     = 0;

    for (u32 i = 0; i < family_count; i++)
    {
        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(g_renderer.physical_device, i, g_renderer.surface, &present_support);

        b32 graphics_support = family_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;

        if (graphics_support && present_support)
        {
            graphics_found = EMBER_TRUE;
            graphics_id    = i;

            present_found = EMBER_TRUE;
            present_id    = i;

            break;
        }

        if (graphics_support && !graphics_found)
        {
            graphics_found = EMBER_TRUE;
            graphics_id    = i;
        }

        if (present_support && !present_found)
        {
            present_found = EMBER_TRUE;
            present_id    = i;
        }
    }

    EMBER_ASSERT(graphics_found && present_found);

    cpu_scratch_end(scratch);

    g_renderer.queue_ids.graphics     = graphics_id;
    g_renderer.queue_ids.presentation = present_id;
}

internal void renderer_create_device()
{
    cpu_scratch_t scratch = cpu_scratch_begin(g_renderer.host_arena);

    u32 queue_count = (g_renderer.queue_ids.graphics == g_renderer.queue_ids.presentation) ? 1 : 2;

    VkDeviceQueueCreateInfo* queue_infos = MEMORY_PUSH_ZERO(scratch.arena, VkDeviceQueueCreateInfo, queue_count);

    f32 queue_priority = 1.0f;
    for (u32 i = 0; i < queue_count; i++)
    {
        queue_infos[i].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_infos[i].queueFamilyIndex = *(((u32 *)(&g_renderer.queue_ids)) + i);
        queue_infos[i].queueCount       = 1;
        queue_infos[i].pQueuePriorities = &queue_priority;
    }

    // TODO(KB): Connect this to the feature check in ...device_is_suitable()
    VkPhysicalDeviceFeatures feats = {0};
    feats.samplerAnisotropy        = VK_TRUE;

    VkPhysicalDeviceVulkan13Features feats_13 = {0};
    feats_13.sType                            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    feats_13.dynamicRendering                 = VK_TRUE;
    feats_13.synchronization2                 = VK_TRUE;

    VkDeviceCreateInfo device_info      = {0};
    device_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext                   = &feats_13;
    device_info.queueCreateInfoCount    = queue_count;
    device_info.pQueueCreateInfos       = queue_infos;
    device_info.pEnabledFeatures        = &feats;
    device_info.enabledExtensionCount   = ARRAY_COUNT(g_device_extensions);
    device_info.ppEnabledExtensionNames = g_device_extensions;
#if RHI_VK_VALIDATIONS_ENABLED
    device_info.enabledLayerCount       = ARRAY_COUNT(g_validation_layers);
    device_info.ppEnabledLayerNames     = g_validation_layers;
#endif

    VkResult create_result = vkCreateDevice(g_renderer.physical_device, &device_info, NULL, &g_renderer.device);
    EMBER_ASSERT(create_result == VK_SUCCESS);

    vkGetDeviceQueue(g_renderer.device, g_renderer.queue_ids.graphics, 0, &g_renderer.graphics_queue);
    vkGetDeviceQueue(g_renderer.device, g_renderer.queue_ids.presentation, 0, &g_renderer.present_queue);

    cpu_scratch_end(scratch);
}

internal void renderer_create_command_pool()
{
    VkCommandPoolCreateInfo pool_info = {0};
    pool_info.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex        = g_renderer.queue_ids.graphics;

    VkResult create_result = vkCreateCommandPool(g_renderer.device, &pool_info, NULL, &g_renderer.command_pool);
    EMBER_ASSERT(create_result == VK_SUCCESS);
}

internal void renderer_create_command_buffers()
{
    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool                 = g_renderer.command_pool;
    alloc_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount          = RENDERER_FRAMES_IN_FLIGHT;

    VkResult alloc_result = vkAllocateCommandBuffers(g_renderer.device, &alloc_info, (VkCommandBuffer *)&g_renderer.command_buffers);
    EMBER_ASSERT(alloc_result == VK_SUCCESS);
}

internal void renderer_create_descriptor_pool()
{
    VkDescriptorPoolSize pool_sizes[1] = {0};
    pool_sizes[0].type                 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[0].descriptorCount      = RENDERER_FRAMES_IN_FLIGHT;

    VkDescriptorPoolCreateInfo pool_info = {0};
    pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount              = ARRAY_COUNT(pool_sizes);
    pool_info.pPoolSizes                 = pool_sizes;
    pool_info.maxSets                    = RENDERER_FRAMES_IN_FLIGHT;

    VkResult vk_result = vkCreateDescriptorPool(g_renderer.device, &pool_info, NULL, &g_renderer.descriptor_pool);
    EMBER_ASSERT(vk_result == VK_SUCCESS);
}

internal void renderer_create_sync_primitives()
{
    VkSemaphoreCreateInfo sem_info = {0};
    sem_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info = {0};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (u32 i = 0; i < RENDERER_FRAMES_IN_FLIGHT; i++)
    {
        VkResult create_result;

        create_result = vkCreateSemaphore(g_renderer.device, &sem_info, NULL, &g_renderer.sem_img_avail[i]);
        EMBER_ASSERT(create_result == VK_SUCCESS);

        create_result = vkCreateSemaphore(g_renderer.device, &sem_info, NULL, &g_renderer.sem_render_end[i]);
        EMBER_ASSERT(create_result == VK_SUCCESS);

        create_result = vkCreateFence(g_renderer.device, &fence_info, NULL, &g_renderer.fence_in_flight[i]);
        EMBER_ASSERT(create_result == VK_SUCCESS);
    }
}

internal void renderer_create_nodes(renderer_node_t* nodes, renderer_ssbo_t* node_ssbo, i32 node_count)
{
    memcpy(g_renderer.node_data + g_renderer.node_count, nodes, node_count * sizeof(renderer_node_t));

    for (i32 i = 0; i < RENDERER_FRAMES_IN_FLIGHT; i++)
    {
        u8* dst = (u8*)g_renderer.resources.ssbo_mapped + i * FRAME_SIZE(GPU_MEM_SIZE_SSBO) + g_renderer.node_count * sizeof(renderer_ssbo_t);

        memcpy(dst, node_ssbo, node_count * sizeof(renderer_ssbo_t));
    }

    g_renderer.node_count += node_count;
}

internal void renderer_create_meshes(mesh_t* m, i32 count)
{
    for (i32 i = 0; i < count; i++)
    {
        u32 vertex_size = m[i].vertex_count * RENDERER_SIZE_VERTEX;
        u32 index_size  = m[i].index_count * RENDERER_SIZE_INDEX;

        renderer_mesh_t* new_mesh = g_renderer.mesh_data + g_renderer.mesh_count;
        renderer_mesh_t* last     = g_renderer.mesh_data + MAX(g_renderer.mesh_count - 1, 0);

        new_mesh->vertex_offset = last->vertex_offset + last->vertex_count;
        new_mesh->index_offset  = last->index_offset + last->index_count;
        new_mesh->vertex_count  = m[i].vertex_count;
        new_mesh->index_count   = m[i].index_count;

        memcpy(g_renderer.resources.stage_mapped, m[i].vertices, vertex_size);
        renderer_resources_copy_buffer(
            g_renderer.resources.stage_buf,
            g_renderer.resources.vertex_buf,
            0,
            new_mesh->vertex_offset * RENDERER_SIZE_VERTEX,
            vertex_size
        );

        memcpy((u8 *)g_renderer.resources.stage_mapped, m[i].indices, index_size);
        renderer_resources_copy_buffer(
            g_renderer.resources.stage_buf,
            g_renderer.resources.index_buf,
            0,
            new_mesh->index_offset * RENDERER_SIZE_INDEX,
            index_size
        );

        g_renderer.mesh_count += 1;
    }
}

internal void renderer_command_buffer_record(renderer_pipeline_t* pipeline, u32 buffer_id, u32 img_id)
{
    VkCommandBufferBeginInfo begin_info = {0};
    begin_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags                    = 0;
    begin_info.pInheritanceInfo         = NULL;

    VkCommandBuffer cmd = g_renderer.command_buffers[buffer_id];

    VkResult cmd_result = vkBeginCommandBuffer(cmd, &begin_info);
    EMBER_ASSERT(cmd_result == VK_SUCCESS);

    VkClearValue clear_value_color = {{ 0.03f, 0.07f, 0.10f, 1.0f }};
    VkClearValue clear_value_depth = { 1.0f, 0.0f };

    VkRenderingAttachmentInfo color_attachment = {0};
    color_attachment.sType                     = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color_attachment.pNext                     = NULL;
    color_attachment.imageView                 = g_renderer.swapchain_img_views[img_id];
    color_attachment.imageLayout               = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    color_attachment.resolveMode               = VK_RESOLVE_MODE_NONE;
    color_attachment.resolveImageView          = VK_NULL_HANDLE;
    color_attachment.resolveImageLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.loadOp                    = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp                   = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.clearValue                = clear_value_color;

    VkRenderingAttachmentInfo depth_attachment = {0};
    depth_attachment.sType                     = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depth_attachment.pNext                     = NULL;
    depth_attachment.imageView                 = g_renderer.resources.depth_image_view;
    depth_attachment.imageLayout               = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depth_attachment.resolveMode               = VK_RESOLVE_MODE_NONE;
    depth_attachment.resolveImageView          = VK_NULL_HANDLE;
    depth_attachment.resolveImageLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.loadOp                    = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp                   = VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment.clearValue                = clear_value_depth;

    VkRenderingInfo rendering_info      = {0};
    rendering_info.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rendering_info.pNext                = NULL;
    rendering_info.flags                = 0;
    rendering_info.renderArea.offset.x  = 0;
    rendering_info.renderArea.offset.y  = 0;
    rendering_info.renderArea.extent    = g_renderer.swapchain_extent;
    rendering_info.layerCount           = 1;
    rendering_info.viewMask             = 0;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachments    = &color_attachment;
    rendering_info.pDepthAttachment     = &depth_attachment;
    rendering_info.pStencilAttachment   = NULL;

    VkViewport viewport = {0};
    viewport.x          = 0.0f;
    viewport.y          = 0.0f;
    viewport.width      = (f32)g_renderer.swapchain_extent.width;
    viewport.height     = (f32)g_renderer.swapchain_extent.height;
    viewport.minDepth   = 0.0f;
    viewport.maxDepth   = 1.0f;

    VkRect2D scissor = {0};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent   = g_renderer.swapchain_extent;

    VkImageMemoryBarrier2 img_barrier           = {0};
    img_barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    img_barrier.srcStageMask                    = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    img_barrier.srcAccessMask                   = 0;
    img_barrier.dstStageMask                    = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    img_barrier.dstAccessMask                   = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    img_barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
    img_barrier.newLayout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    img_barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    img_barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    img_barrier.image                           = g_renderer.swapchain_images[img_id];
    img_barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    img_barrier.subresourceRange.baseMipLevel   = 0;
    img_barrier.subresourceRange.levelCount     = 1;
    img_barrier.subresourceRange.baseArrayLayer = 0;
    img_barrier.subresourceRange.layerCount     = 1;

    VkDependencyInfo dep_info        = {0};
    dep_info.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dep_info.dependencyFlags         = 0;
    dep_info.imageMemoryBarrierCount = 1;
    dep_info.pImageMemoryBarriers    = &img_barrier;

    vkCmdPipelineBarrier2(cmd, &dep_info);

    vkCmdBeginRendering(cmd, &rendering_info);

    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphics_pipeline);

    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(cmd, 0, 1, &g_renderer.resources.vertex_buf, offsets);
    vkCmdBindIndexBuffer(cmd, g_renderer.resources.index_buf, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(
        cmd,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline->graphics_pipeline_layout,
        0,
        1,
        &pipeline->descriptor_sets[buffer_id],
        0,
        NULL
    );

    u64 offset_dcmd = buffer_id * FRAME_SIZE(GPU_MEM_SIZE_DCMD);
    u64 offset_draw = buffer_id * FRAME_SIZE(GPU_MEM_SIZE_DRAW);

    i32 idx = 0;
    for (i32 i = 0; i < g_renderer.node_count; i++)
    {
        renderer_node_t node = g_renderer.node_data[i];

        for (i32 j = 0; j < node.mesh_count; j++)
        {
            renderer_mesh_t mesh = g_renderer.mesh_data[node.mesh_id + j];

            renderer_dcmd_data_t* dcmd_data = (renderer_dcmd_data_t *)((u8 *)g_renderer.resources.dcmd_mapped + offset_dcmd);
            renderer_draw_data_t* draw_data = (renderer_draw_data_t *)((u8 *)g_renderer.resources.draw_mapped + offset_draw);

            dcmd_data[idx].indexCount    = mesh.index_count;
            dcmd_data[idx].firstIndex    = mesh.index_offset;
            dcmd_data[idx].vertexOffset  = mesh.vertex_offset;
            dcmd_data[idx].instanceCount = 1;
            dcmd_data[idx].firstInstance = idx;

            draw_data[idx].transform_id  = i;

            idx += 1;
        }
    }

    vkCmdDrawIndexedIndirect(cmd, g_renderer.resources.dcmd_buf, offset_dcmd, idx, sizeof(renderer_dcmd_data_t));

    vkCmdEndRendering(cmd);

    VkImageMemoryBarrier2 img_barrier_end           = {0};
    img_barrier_end.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    img_barrier_end.srcStageMask                    = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    img_barrier_end.srcAccessMask                   = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    img_barrier_end.dstStageMask                    = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
    img_barrier_end.dstAccessMask                   = 0;
    img_barrier_end.oldLayout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    img_barrier_end.newLayout                       = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    img_barrier_end.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    img_barrier_end.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    img_barrier_end.image                           = g_renderer.swapchain_images[img_id];
    img_barrier_end.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    img_barrier_end.subresourceRange.baseMipLevel   = 0;
    img_barrier_end.subresourceRange.levelCount     = 1;
    img_barrier_end.subresourceRange.baseArrayLayer = 0;
    img_barrier_end.subresourceRange.layerCount     = 1;

    VkDependencyInfo dep_info_end        = {0};
    dep_info_end.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dep_info_end.dependencyFlags         = 0;
    dep_info_end.imageMemoryBarrierCount = 1;
    dep_info_end.pImageMemoryBarriers    = &img_barrier_end;

    vkCmdPipelineBarrier2(cmd, &dep_info_end);

    cmd_result = vkEndCommandBuffer(cmd);
    EMBER_ASSERT(cmd_result == VK_SUCCESS);
}

internal b32 renderer_check_validation_layers()
{
    cpu_scratch_t scratch = cpu_scratch_begin(g_renderer.host_arena);

    u32 layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkLayerProperties* layer_props = MEMORY_PUSH(scratch.arena, VkLayerProperties, layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, layer_props);

    b32 layers_found = EMBER_TRUE;
    for (u32 i = 0; i < ARRAY_COUNT(g_validation_layers); i++)
    {
        b32 layer_found = EMBER_FALSE;
        for (u32 j = 0; j < layer_count; j++)
        {
            if (strcmp(g_validation_layers[i], layer_props[j].layerName) == 0)
            {
                layer_found = EMBER_TRUE;
                break;
            }
        }

        layers_found &= layer_found;
    }

    cpu_scratch_end(scratch);

    return layers_found;
}

internal b32 renderer_check_instance_extensions()
{
    cpu_scratch_t scratch = cpu_scratch_begin(g_renderer.host_arena);

    u32 ext_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &ext_count, NULL);

    VkExtensionProperties* ext_props = MEMORY_PUSH(scratch.arena, VkExtensionProperties, ext_count);
    vkEnumerateInstanceExtensionProperties(NULL, &ext_count, ext_props);

    b32 exts_found = EMBER_TRUE;
    for (u32 i = 0; i < ARRAY_COUNT(g_instance_extensions); i++)
    {
        b32 ext_found = EMBER_FALSE;
        for (u32 j = 0; j < ext_count; j++)
        {
            if (strcmp(g_instance_extensions[i], ext_props[j].extensionName) == 0)
            {
                ext_found = EMBER_TRUE;
                break;
            }
        }

        exts_found &= ext_found;
    }

    cpu_scratch_end(scratch);

    return exts_found;
}

internal b32 renderer_check_device_extensions(VkPhysicalDevice device)
{
    cpu_scratch_t scratch = cpu_scratch_begin(g_renderer.host_arena);

    u32 ext_count = 0;
    vkEnumerateDeviceExtensionProperties(device, NULL, &ext_count, NULL);

    VkExtensionProperties* ext_props = MEMORY_PUSH(scratch.arena, VkExtensionProperties, ext_count);
    vkEnumerateDeviceExtensionProperties(device, NULL, &ext_count, ext_props);

    b32 exts_found = EMBER_TRUE;
    for (u32 i = 0; i < ARRAY_COUNT(g_device_extensions); i++)
    {
        b32 ext_found = EMBER_FALSE;
        for (u32 j = 0; j < ext_count; j++)
        {
            if (strcmp(g_device_extensions[i], ext_props[j].extensionName) == 0)
            {
                ext_found = EMBER_TRUE;
                break;
            }
        }

        exts_found &= ext_found;
    }

    cpu_scratch_end(scratch);

    return exts_found;
}

internal b32 renderer_device_is_suitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties2 props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceVulkan13Features feats_13 = {0};
    feats_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

    VkPhysicalDeviceFeatures2 feats ={0};
    feats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    feats.pNext = &feats_13;

    vkGetPhysicalDeviceProperties2(device, &props);
    vkGetPhysicalDeviceFeatures2(device, &feats);

    b32 exts_supported = renderer_check_device_extensions(device);
    b32 prop_supported = (props.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
    b32 feat_supported = feats.features.samplerAnisotropy && feats_13.dynamicRendering && feats_13.synchronization2;

    b32 result =
        exts_supported &&
        prop_supported &&
        feat_supported;

    return result;
}
