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
) {
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = 2,
        .imageFormat = format,
        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent = {.width = width, .height = height},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &queueFamilyIndex,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .oldSwapchain = oldSwapchain,
    };

    VkSwapchainKHR handle = 0;
    VK_CHECK(vkCreateSwapchainKHR(device, &swapchainCreateInfo, 0, &handle));

    Swapchain swapchain = {
        .handle = handle,
        .width = width,
        .height = height,
    };

    vkGetSwapchainImagesKHR(device, handle, &swapchain.imageCount, 0);  // TODO: is this needed?
    vkGetSwapchainImagesKHR(device, handle, &swapchain.imageCount, swapchain.images);

    for (int i = 0; i < swapchain.imageCount; i++) {
        swapchain.imageViews[i] = createImageView(device, swapchain.images[i], format);
    }

    for (int i = 0; i < swapchain.imageCount; i++) {
        swapchain.framebuffers[i] = createFramebuffer(device, renderPass, swapchain.imageViews[i], width, height);
    }

    return swapchain;
}

void destroySwapchain(VkDevice device, Swapchain swapchain) {
    for (int i = 0; i < swapchain.imageCount; i++) {
        vkDestroyImageView(device, swapchain.imageViews[i], 0);
    }

    for (int i = 0; i < swapchain.imageCount; i++) {
        vkDestroyFramebuffer(device, swapchain.framebuffers[i], 0);
    }

    vkDestroySwapchainKHR(device, swapchain.handle, 0);
}

Swapchain syncSwapchain(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    u32 queueFamilyIndex,
    VkFormat format,
    VkRenderPass renderPass,
    Swapchain swapchain
) {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));

    u32 newWidth = surfaceCapabilities.currentExtent.width;
    u32 newHeight = surfaceCapabilities.currentExtent.height;

    if (swapchain.width == newWidth && swapchain.height == newHeight) {
        return swapchain;
    }

    Swapchain newSwapchain =
        createSwapchain(device, queueFamilyIndex, surface, renderPass, format, newWidth, newHeight, swapchain.handle);

    VK_CHECK(vkDeviceWaitIdle(device));

    destroySwapchain(device, swapchain);

    return newSwapchain;
}

#endif /* __SWAPCHAIN__ */