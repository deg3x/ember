internal void renderer_swapchain_init(platform_hnd_t window_handle)
{
    VkSurfaceFormatKHR swap_format = renderer_swapchain_find_format();
    VkPresentModeKHR swap_present  = renderer_swapchain_find_present();
    VkExtent2D swap_extent         = renderer_swapchain_find_extent(window_handle);

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g_renderer.physical_device, g_renderer.surface, &capabilities);

    u32 img_count = RENDERER_SWAP_IMG_COUNT;

    EMBER_ASSERT(img_count >= capabilities.minImageCount);
    EMBER_ASSERT(img_count <= capabilities.maxImageCount || capabilities.maxImageCount == 0);

    VkSwapchainCreateInfoKHR swap_info = {0};
    swap_info.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swap_info.surface                  = g_renderer.surface;
    swap_info.minImageCount            = img_count;
    swap_info.imageFormat              = swap_format.format;
    swap_info.imageColorSpace          = swap_format.colorSpace;
    swap_info.imageExtent              = swap_extent;
    swap_info.imageArrayLayers         = 1;
    swap_info.preTransform             = capabilities.currentTransform;
    swap_info.presentMode              = swap_present;
    swap_info.clipped                  = VK_TRUE;
    swap_info.oldSwapchain             = VK_NULL_HANDLE;
    swap_info.compositeAlpha           = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swap_info.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (g_renderer.queue_ids.graphics != g_renderer.queue_ids.presentation)
    {
        swap_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        swap_info.queueFamilyIndexCount = 2;
        swap_info.pQueueFamilyIndices   = (u32 *)(&g_renderer.queue_ids);
    }
    else
    {
        swap_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        swap_info.queueFamilyIndexCount = 0;
        swap_info.pQueueFamilyIndices   = NULL;
    }

    VkResult create_result = vkCreateSwapchainKHR(g_renderer.device, &swap_info, NULL, &g_renderer.swapchain);
    EMBER_ASSERT(create_result == VK_SUCCESS);

    g_renderer.swapchain_extent    = swap_extent;
    g_renderer.swapchain_img_fmt   = swap_format.format;
    g_renderer.swapchain_images    = MEMORY_PUSH(g_renderer.host_arena, VkImage, img_count);
    g_renderer.swapchain_img_views = MEMORY_PUSH(g_renderer.host_arena, VkImageView, img_count);

    vkGetSwapchainImagesKHR(g_renderer.device, g_renderer.swapchain, &img_count, g_renderer.swapchain_images);

    for (u32 i = 0; i < img_count; i++)
    {
        renderer_resources_create_image_view(
            g_renderer.swapchain_images[i],
            &g_renderer.swapchain_img_views[i],
            g_renderer.swapchain_img_fmt,
            VK_IMAGE_ASPECT_COLOR_BIT
        );
    }
}

internal void renderer_swapchain_destroy()
{
    for (u32 i = 0; i < RENDERER_SWAP_IMG_COUNT; i++)
    {
        vkDestroyImageView(g_renderer.device, g_renderer.swapchain_img_views[i], NULL);
    }

    vkDestroySwapchainKHR(g_renderer.device, g_renderer.swapchain, NULL);
}

internal void renderer_swapchain_recreate(platform_hnd_t window_handle)
{
    while(platform_gfx_wnd_is_minimized(window_handle))
    {
        platform_gfx_process_events();
    }

    vkDeviceWaitIdle(g_renderer.device);

    for (u32 i = 0; i < RENDERER_SWAP_IMG_COUNT; i++)
    {
        vkDestroyImageView(g_renderer.device, g_renderer.swapchain_img_views[i], NULL);
    }
    vkDestroySwapchainKHR(g_renderer.device, g_renderer.swapchain, NULL);

    vkDestroyImageView(g_renderer.device, g_renderer.resources.depth_image_view, NULL);
    vkDestroyImage(g_renderer.device, g_renderer.resources.depth_image, NULL);

    gpu_arena_free(&g_renderer.gpu_arena, g_renderer.resources.depth_mem);

    renderer_swapchain_init(window_handle);
    renderer_resources_create_depth();
}

internal VkSurfaceFormatKHR renderer_swapchain_find_format()
{
    cpu_scratch_t scratch = cpu_scratch_begin(g_renderer.host_arena);

    u32 format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(g_renderer.physical_device, g_renderer.surface, &format_count, NULL);

    EMBER_ASSERT(format_count > 0);

    VkSurfaceFormatKHR* formats = MEMORY_PUSH(scratch.arena, VkSurfaceFormatKHR, format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(g_renderer.physical_device, g_renderer.surface, &format_count, formats);

    VkSurfaceFormatKHR result = {
        VK_FORMAT_B8G8R8_SRGB,
        VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    };

    if (format_count == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
    {
        cpu_scratch_end(scratch);

        return result;
    }

    for (u32 i = 0; i < format_count; i++)
    {
        if (formats[i].format == result.format && formats[i].colorSpace == result.colorSpace)
        {
            cpu_scratch_end(scratch);

            return result;
        }
    }

    result = formats[0];

    cpu_scratch_end(scratch);

    return result;
}

internal VkPresentModeKHR renderer_swapchain_find_present()
{
    cpu_scratch_t scratch = cpu_scratch_begin(g_renderer.host_arena);

    u32 present_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(g_renderer.physical_device, g_renderer.surface, &present_count, NULL);

    VkPresentModeKHR* present_modes = MEMORY_PUSH(scratch.arena, VkPresentModeKHR, present_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(g_renderer.physical_device, g_renderer.surface, &present_count, present_modes);

    for (u32 i = 0; i < present_count; i++)
    {
        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            cpu_scratch_end(scratch);
            return present_modes[i];
        }
    }

    // NOTE(KB): VK_PRESENT_MODE_FIFO_KHR is the only one guaranteed to exist
    cpu_scratch_end(scratch);
    return VK_PRESENT_MODE_FIFO_KHR;
}

internal VkExtent2D renderer_swapchain_find_extent(platform_hnd_t window_handle)
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g_renderer.physical_device, g_renderer.surface, &capabilities);

    if (capabilities.currentExtent.width != U32_MAX)
    {
        return capabilities.currentExtent;
    }

    platform_wnd_size_t client_size = platform_gfx_wnd_client_get_size(window_handle);

    VkExtent2D extent;

    extent.width  = CLAMP(client_size.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = CLAMP(client_size.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return extent;
}
