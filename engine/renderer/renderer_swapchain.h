#ifndef RENDERER_SWAPCHAIN_H
#define RENDERER_SWAPCHAIN_H

internal void renderer_swapchain_init(platform_hnd_t window_handle);
internal void renderer_swapchain_destroy();

internal void               renderer_swapchain_recreate(platform_hnd_t window_handle);
internal VkSurfaceFormatKHR renderer_swapchain_find_format();
internal VkPresentModeKHR   renderer_swapchain_find_present();
internal VkExtent2D         renderer_swapchain_find_extent(platform_hnd_t window_handle);

#endif // RENDERER_SWAPCHAIN_H
