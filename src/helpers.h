#ifndef __HELPERS__
#define __HELPERS__

#include "base.h"

VkImageView createImageView(VkDevice device, VkImage image, VkFormat format) {
    VkImageSubresourceRange subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
    };

    VkImageViewCreateInfo imageViewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange = subresourceRange,
    };

    VkImageView imageView = 0;
    VK_CHECK(vkCreateImageView(device, &imageViewCreateInfo, 0, &imageView));

    return imageView;
}

VkRenderPass createRenderPass(VkDevice device, VkFormat format) {
    VkAttachmentDescription colorAttachmentDescription = {
        .format = format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkAttachmentReference colorAttachment = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachment,
    };

    VkRenderPassCreateInfo rpCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colorAttachmentDescription,
        .subpassCount = 1,
        .pSubpasses = &subpass,
    };

    VkRenderPass renderPass = 0;
    VK_CHECK(vkCreateRenderPass(device, &rpCreateInfo, 0, &renderPass));

    return renderPass;
}

VkFramebuffer createFramebuffer(
    VkDevice device, VkRenderPass renderPass, VkImageView imageView, u32 width, u32 height
) {
    VkFramebufferCreateInfo fbCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = renderPass,
        .attachmentCount = 1,
        .pAttachments = &imageView,
        .width = width,
        .height = height,
        .layers = 1,
    };

    VkFramebuffer framebuffer = 0;
    VK_CHECK(vkCreateFramebuffer(device, &fbCreateInfo, 0, &framebuffer));

    return framebuffer;
}

// NOTE: MEMORY
u32 findMemoryTypeIndex(
    VkPhysicalDeviceMemoryProperties memoryProperties, u32 memoryTypeBits, VkMemoryPropertyFlags flags
) {
    for (int i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if ((memoryTypeBits & (1 << i)) != 0 && (memoryProperties.memoryTypes[i].propertyFlags & flags) == flags) {
            return i;
        }
    }
    assert(!"Could not find memory type index!");
}

// NOTE: COMMAND BUFFERS
VkCommandPool createCommandPool(VkDevice device, u32 queueFamilyIndex) {
    VkCommandPoolCreateInfo commandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = queueFamilyIndex,
    };

    VkCommandPool commandPool = 0;
    VK_CHECK(vkCreateCommandPool(device, &commandPoolCreateInfo, 0, &commandPool));

    return commandPool;
}

VkCommandBuffer allocateCommandBuffer(VkDevice device, VkCommandPool commandPool) {
    VkCommandBufferAllocateInfo cmdBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer cmdBuffer = 0;
    VK_CHECK(vkAllocateCommandBuffers(device, &cmdBufferAllocateInfo, &cmdBuffer));

    return cmdBuffer;
}

// NOTE: SYNC OBJECTS
VkSemaphore createSemaphore(VkDevice device) {
    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkSemaphore semaphore = 0;
    VK_CHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, 0, &semaphore));

    return semaphore;
}

void pipelineImageBarrier(
    VkCommandBuffer cmdBuffer,
    VkImage image,
    VkPipelineStageFlags srcStageMask,
    VkAccessFlags srcAccessMask,
    VkImageLayout oldLayout,
    VkPipelineStageFlags dstStageMask,
    VkAccessFlags dstAccessMask,
    VkImageLayout newLayout
) {
    VkImageSubresourceRange subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseArrayLayer = 0,
        .baseMipLevel = 0,
        .levelCount = VK_REMAINING_MIP_LEVELS,
        .layerCount = VK_REMAINING_ARRAY_LAYERS,
    };

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = srcAccessMask,
        .dstAccessMask = dstAccessMask,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = subresourceRange,
    };
    vkCmdPipelineBarrier(cmdBuffer, srcStageMask, dstStageMask, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &barrier);
}

#endif /* __HELPERS__ */
