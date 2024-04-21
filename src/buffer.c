#include "buffer.h"

Buffer createBuffer(
    VkDevice device,
    VkPhysicalDeviceMemoryProperties memoryProperties,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags memoryPropertyFlags
) {
    VkBuffer handle = 0;
    VK_CHECK(vkCreateBuffer(
        device,
        &(VkBufferCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = 0,
        },
        0,
        &handle
    ));

    VkMemoryRequirements bufferMemoryRequirements;
    vkGetBufferMemoryRequirements(device, handle, &bufferMemoryRequirements);

    u32 memoryTypeIndex = findMemoryTypeIndex(memoryProperties, bufferMemoryRequirements.memoryTypeBits, memoryPropertyFlags);

    VkDeviceMemory memory = 0;
    VK_CHECK(vkAllocateMemory(
        device,
        &(VkMemoryAllocateInfo) {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = bufferMemoryRequirements.size,
            .memoryTypeIndex = memoryTypeIndex,
        },
        0,
        &memory
    ));

    VK_CHECK(vkBindBufferMemory(device, handle, memory, 0));

    void* data = 0;
    if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == 0) {
        VK_CHECK(vkMapMemory(device, memory, 0, size, 0, &data));
    }

    Buffer buffer = {
        .handle = handle,
        .memory = memory,
        .data = data,
        .size = size,
    };
    return buffer;
}

void destroyBuffer(VkDevice device, Buffer buffer) {
    if (buffer.data) {
        vkUnmapMemory(device, buffer.memory);
    }
    vkDestroyBuffer(device, buffer.handle, 0);
    vkFreeMemory(device, buffer.memory, 0);

    buffer.handle = VK_NULL_HANDLE;
    buffer.memory = VK_NULL_HANDLE;
    buffer.data = 0;
    buffer.size = 0;
}

void uploadDataToBuffer(
    VkDevice device,
    VkQueue queue,
    VkCommandPool cmdPool,
    VkCommandBuffer cmdBuffer,
    Buffer scratch,
    Buffer buffer,
    void* data,
    usize size
) {
    assert(scratch.size >= size);
    assert(buffer.size >= size);

    assert(scratch.data);
    memcpy(scratch.data, data, size);

    VK_CHECK(vkResetCommandPool(device, cmdPool, 0));

    beginCommandBuffer(cmdBuffer);

    vkCmdCopyBuffer(
        cmdBuffer,
        scratch.handle,
        buffer.handle,
        1,
        &(VkBufferCopy) {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = size,
        }
    );

    vkEndCommandBuffer(cmdBuffer);

    VK_CHECK(vkQueueSubmit(
        queue,
        1,
        &(VkSubmitInfo) {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &cmdBuffer,
        },
        VK_NULL_HANDLE
    ));

    VK_CHECK(vkDeviceWaitIdle(device));
}
