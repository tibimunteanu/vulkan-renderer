#ifndef __RENDERER__
#define __RENDERER__

#include "instance.h"
#include "device.h"
#include "shader.h"
#include "buffer.h"
#include "swapchain.h"

typedef struct Renderer {
    GLFWwindow* window;
    VkInstance instance;
    VkSurfaceKHR surface;

    PhysicalDevice physicalDevice;
    VkDevice device;
    u32 queueFamilyIndex;
    VkQueue queue;

    i32 width, height;
    VkFormat format;

    VkRenderPass renderPass;
    Swapchain swapchain;
    VkCommandPool commandPool;
    VkCommandBuffer cmdBuffer;
    VkSemaphore acquireSemaphore;
    VkSemaphore releaseSemaphore;

    VkShaderModule vs;
    VkShaderModule fs;

    VkPipelineLayout layout;
    VkPipeline pipeline;

    Buffer vb;
    Buffer ib;
} Renderer;

static Renderer renderer = {};

void createRenderer() {
    assert(glfwInit());

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    renderer.window = glfwCreateWindow(960, 540, "Vulkan Renderer", 0, 0);
    assert(renderer.window);

    VK_CHECK(volkInitialize());

    renderer.instance = createInstance();
    volkLoadInstance(renderer.instance);

    renderer.surface = 0;
    VK_CHECK(glfwCreateWindowSurface(renderer.instance, renderer.window, 0, &renderer.surface));

    renderer.physicalDevice = pickPhysicalDevice(renderer.instance, renderer.surface);

    renderer.device = createDevice(renderer.physicalDevice);
    volkLoadDevice(renderer.device);

    renderer.queueFamilyIndex = renderer.physicalDevice.queueFamilyIndex;

    renderer.queue = 0;
    vkGetDeviceQueue(renderer.device, renderer.queueFamilyIndex, 0, &renderer.queue);
    assert(renderer.queue);

    glfwGetFramebufferSize(renderer.window, &renderer.width, &renderer.height);

    // TODO: check for support
    renderer.format = VK_FORMAT_R8G8B8A8_UNORM;

    renderer.renderPass = createRenderPass(renderer.device, renderer.format);
    renderer.swapchain = createSwapchain(
        renderer.device,
        renderer.queueFamilyIndex,
        renderer.surface,
        renderer.renderPass,
        renderer.format,
        renderer.width,
        renderer.height,
        VK_NULL_HANDLE
    );
    renderer.commandPool = createCommandPool(renderer.device, renderer.queueFamilyIndex);
    renderer.cmdBuffer = allocateCommandBuffer(renderer.device, renderer.commandPool);
    renderer.acquireSemaphore = createSemaphore(renderer.device);
    renderer.releaseSemaphore = createSemaphore(renderer.device);

    renderer.vs = loadShader(renderer.device, "assets/shaders/triangle.vert.spv");
    renderer.fs = loadShader(renderer.device, "assets/shaders/triangle.frag.spv");

    renderer.layout = createPipelineLayout(renderer.device);
    renderer.pipeline = createPipeline(renderer.device, renderer.layout, renderer.vs, renderer.fs, renderer.renderPass);

    renderer.vb = createBuffer(
        renderer.device,
        renderer.physicalDevice.memoryProperties,
        MB(64),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    renderer.ib = createBuffer(
        renderer.device,
        renderer.physicalDevice.memoryProperties,
        MB(64),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    // TODO: replace with uploadDataToBuffer()
    assert(renderer.vb.size >= sizeof(vertices));
    memcpy(renderer.vb.data, vertices, sizeof(vertices));

    assert(renderer.ib.size >= sizeof(indices));
    memcpy(renderer.ib.data, indices, sizeof(indices));
}

void destroyRenderer() {
    VK_CHECK(vkDeviceWaitIdle(renderer.device));

    destroyBuffer(renderer.device, renderer.ib);
    destroyBuffer(renderer.device, renderer.vb);
    vkDestroyPipelineLayout(renderer.device, renderer.layout, 0);
    vkDestroyPipeline(renderer.device, renderer.pipeline, 0);
    vkDestroyShaderModule(renderer.device, renderer.fs, 0);
    vkDestroyShaderModule(renderer.device, renderer.vs, 0);
    vkDestroySemaphore(renderer.device, renderer.releaseSemaphore, 0);
    vkDestroySemaphore(renderer.device, renderer.acquireSemaphore, 0);
    vkDestroyCommandPool(renderer.device, renderer.commandPool, 0);
    destroySwapchain(renderer.device, renderer.swapchain);
    vkDestroyRenderPass(renderer.device, renderer.renderPass, 0);
    vkDestroyDevice(renderer.device, 0);
    vkDestroySurfaceKHR(renderer.instance, renderer.surface, 0);
    vkDestroyInstance(renderer.instance, 0);

    glfwDestroyWindow(renderer.window);

    glfwTerminate();
}

#endif /* __RENDERER__ */
