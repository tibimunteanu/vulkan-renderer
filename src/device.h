#ifndef __DEVICE__
#define __DEVICE__

#include "base.h"

typedef struct PhysicalDevice {
    VkPhysicalDevice handle;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memoryProperties;
    u32 queueFamilyIndex;
} PhysicalDevice;

PhysicalDevice pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);

VkDevice createDevice(PhysicalDevice physicalDevice);

VkFormat getFormat();

#endif /* __DEVICE__ */
