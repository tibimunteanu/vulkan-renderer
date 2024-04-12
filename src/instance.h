#ifndef __INSTANCE__
#define __INSTANCE__

#include "base.h"

static const char* instanceExtensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
};

VkInstance createInstance() {
    VkApplicationInfo applicationInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Vulkan Renderer",
        .applicationVersion = VK_VERSION_1_0,
        .pEngineName = "Vulkan Engine",
        .engineVersion = VK_VERSION_1_0,
        .apiVersion = VK_API_VERSION_1_1,
    };

    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = 0,
        .enabledExtensionCount = ARRAY_SIZE(instanceExtensions),
        .ppEnabledExtensionNames = instanceExtensions,
    };

    VkInstance instance = 0;
    VK_CHECK(vkCreateInstance(&instanceCreateInfo, 0, &instance));

    return instance;
}

#endif /* __INSTANCE__ */