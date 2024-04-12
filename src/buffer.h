#ifndef __BUFFER__
#define __BUFFER__

#include "base.h"
#include "helpers.h"

typedef struct Buffer {
    VkBuffer handle;
    VkDeviceMemory memory;
    void* data;
    usize size;
} Buffer;

Buffer createBuffer(
    VkDevice device,
    VkPhysicalDeviceMemoryProperties memoryProperties,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags memoryPropertyFlags
) {
    VkBufferCreateInfo bufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = 0,
    };

    VkBuffer handle = 0;
    VK_CHECK(vkCreateBuffer(device, &bufferCreateInfo, 0, &handle));

    VkMemoryRequirements bufferMemoryRequirements;
    vkGetBufferMemoryRequirements(device, handle, &bufferMemoryRequirements);

    u32 memoryTypeIndex =
        findMemoryTypeIndex(memoryProperties, bufferMemoryRequirements.memoryTypeBits, memoryPropertyFlags);

    VkMemoryAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = bufferMemoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };

    VkDeviceMemory memory = 0;
    VK_CHECK(vkAllocateMemory(device, &allocateInfo, 0, &memory));

    VK_CHECK(vkBindBufferMemory(device, handle, memory, 0));

    void* data = 0;
    VK_CHECK(vkMapMemory(device, memory, 0, size, 0, &data));

    Buffer buffer = {
        .handle = handle,
        .memory = memory,
        .data = data,
        .size = size,
    };
    return buffer;
}

void destroyBuffer(VkDevice device, Buffer buffer) {
    vkUnmapMemory(device, buffer.memory);
    vkDestroyBuffer(device, buffer.handle, 0);
    vkFreeMemory(device, buffer.memory, 0);

    buffer.handle = VK_NULL_HANDLE;
    buffer.memory = VK_NULL_HANDLE;
    buffer.data = 0;
    buffer.size = 0;
}

#endif /* __BUFFER__ */