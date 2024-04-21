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
);

void destroyBuffer(VkDevice device, Buffer buffer);

void uploadDataToBuffer(
    VkDevice device,
    VkQueue queue,
    VkCommandPool cmdPool,
    VkCommandBuffer cmdBuffer,
    Buffer scratch,
    Buffer buffer,
    void* data,
    usize size
);

#endif /* __BUFFER__ */
