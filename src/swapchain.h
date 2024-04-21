#ifndef __SWAPCHAIN__
#define __SWAPCHAIN__

#include "base.h"
#include "helpers.h"

typedef struct Swapchain {
    VkSwapchainKHR handle;
    u32 width;
    u32 height;
    u32 imageCount;
    VkImage images[16];
    VkImageView imageViews[16];
    VkFramebuffer framebuffers[16];
} Swapchain;

Swapchain createSwapchain(
    VkDevice device,
    u32 queueFamilyIndex,
    VkSurfaceKHR surface,
    VkRenderPass renderPass,
    VkFormat format,
    u32 width,
    u32 height,
    VkSwapchainKHR oldSwapchain
);

void destroySwapchain(VkDevice device, Swapchain swapchain);

Swapchain syncSwapchain(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    u32 queueFamilyIndex,
    VkFormat format,
    VkRenderPass renderPass,
    Swapchain swapchain
);

#endif /* __SWAPCHAIN__ */