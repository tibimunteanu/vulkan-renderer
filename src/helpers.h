#ifndef __HELPERS__
#define __HELPERS__

#include "base.h"

VkImageView createImageView(VkDevice device, VkImage image, VkFormat format);

VkRenderPass createRenderPass(VkDevice device, VkFormat format);

VkFramebuffer createFramebuffer(VkDevice device, VkRenderPass renderPass, VkImageView imageView, u32 width, u32 height);

// NOTE: MEMORY
u32 findMemoryTypeIndex(VkPhysicalDeviceMemoryProperties memoryProperties, u32 memoryTypeBits, VkMemoryPropertyFlags flags);

// NOTE: COMMAND BUFFERS
VkCommandPool createCommandPool(VkDevice device, u32 queueFamilyIndex);

VkCommandBuffer allocateCommandBuffer(VkDevice device, VkCommandPool cmdPool);

void beginCommandBuffer(VkCommandBuffer cmdBuffer);

void setViewportAndScissor(VkCommandBuffer cmdBuffer, u32 width, u32 height);

// NOTE: SYNC OBJECTS
VkSemaphore createSemaphore(VkDevice device);

void pipelineImageBarrier(
    VkCommandBuffer cmdBuffer,
    VkImage image,
    VkPipelineStageFlags srcStageMask,
    VkAccessFlags srcAccessMask,
    VkImageLayout oldLayout,
    VkPipelineStageFlags dstStageMask,
    VkAccessFlags dstAccessMask,
    VkImageLayout newLayout
);

#endif /* __HELPERS__ */
