#include "device.h"

static const char* deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

PhysicalDevice pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface) {
    u32 physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, 0);

    VkPhysicalDevice physicalDevices[16];
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices);

    for (int i = 0; i < physicalDeviceCount; i++) {
        VkPhysicalDevice handle = physicalDevices[i];

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(handle, &properties);

#ifndef VK_USE_PLATFORM_METAL_EXT
        // ensure discrete gpu
        if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            continue;
        }
#endif

        // ensure queue support
        u32 qfPropertyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(handle, &qfPropertyCount, 0);

        VkQueueFamilyProperties qfProperties[32];
        vkGetPhysicalDeviceQueueFamilyProperties(handle, &qfPropertyCount, qfProperties);

        u32 queueFamilyIndex = -1;
        for (int q = 0; q < qfPropertyCount; q++) {
            VkQueueFlags flags = qfProperties[q].queueFlags;

            b32 has_graphics = flags & VK_QUEUE_GRAPHICS_BIT;
            b32 has_compute = flags & VK_QUEUE_COMPUTE_BIT;
            b32 has_transfer = flags & VK_QUEUE_TRANSFER_BIT;

            if (has_graphics && has_compute && has_transfer) {
                queueFamilyIndex = q;
            }
        }
        if (queueFamilyIndex == -1) {
            continue;
        }

        // ensure present support
        b32 presentSupported = false;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(handle, queueFamilyIndex, surface, &presentSupported));
        if (!presentSupported) {
            continue;
        }

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(handle, &features);

        // ensure feature support
        if (!features.robustBufferAccess) {
            continue;
        }

        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(handle, &memoryProperties);

        printf("%s\n", properties.deviceName);

        PhysicalDevice physicalDevice = {
            .handle = handle,
            .properties = properties,
            .features = features,
            .memoryProperties = memoryProperties,
            .queueFamilyIndex = queueFamilyIndex,
        };
        return physicalDevice;
    }

    assert(!"Could not find suitable device!");
}

VkDevice createDevice(PhysicalDevice physicalDevice) {
    f32 queuePriorities = 1;

    VkDeviceQueueCreateInfo queueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = physicalDevice.queueFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriorities,
    };

    VkPhysicalDeviceFeatures enabledFeatures = {
        .robustBufferAccess = true,
    };

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCreateInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = 0,
        .enabledExtensionCount = ARRAY_SIZE(deviceExtensions),
        .ppEnabledExtensionNames = deviceExtensions,
        .pEnabledFeatures = &enabledFeatures,
    };

    VkDevice device = 0;
    VK_CHECK(vkCreateDevice(physicalDevice.handle, &deviceCreateInfo, 0, &device));

    return device;
}

VkFormat getFormat() {
    // TODO: check for support
    return VK_FORMAT_R8G8B8A8_UNORM;
}
