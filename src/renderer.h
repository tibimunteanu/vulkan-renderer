#ifndef __RENDERER__
#define __RENDERER__

#include "instance.h"
#include "device.h"
#include "shader.h"
#include "buffer.h"
#include "swapchain.h"

static VkInstance instance;
static VkSurfaceKHR surface;
static PhysicalDevice physicalDevice;
static VkDevice device;
static u32 queueFamilyIndex;
static VkQueue queue;

static i32 width, height;
static VkFormat format;

static VkRenderPass renderPass;
static Swapchain swapchain;
static VkCommandPool cmdPool;
static VkCommandBuffer cmdBuffer;
static VkSemaphore acquireSemaphore;
static VkSemaphore releaseSemaphore;

static VkShaderModule vs;
static VkShaderModule fs;

static VkPipelineLayout layout;
static VkPipeline pipeline;

static Buffer scratch;
static Buffer vb;
static Buffer ib;

static u32 imageIndex;

void createRenderer(GLFWwindow* window) {
    VK_CHECK(volkInitialize());

    instance = createInstance();
    volkLoadInstance(instance);

    VK_CHECK(glfwCreateWindowSurface(instance, window, 0, &surface));

    physicalDevice = pickPhysicalDevice(instance, surface);

    device = createDevice(physicalDevice);
    volkLoadDevice(device);

    queueFamilyIndex = physicalDevice.queueFamilyIndex;
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);
    assert(queue);

    glfwGetFramebufferSize(window, &width, &height);

    // TODO: check for support
    format = VK_FORMAT_R8G8B8A8_UNORM;

    renderPass = createRenderPass(device, format);
    swapchain = createSwapchain(device, queueFamilyIndex, surface, renderPass, format, width, height, VK_NULL_HANDLE);
    cmdPool = createCommandPool(device, queueFamilyIndex);
    cmdBuffer = allocateCommandBuffer(device, cmdPool);
    acquireSemaphore = createSemaphore(device);
    releaseSemaphore = createSemaphore(device);

    vs = loadShader(device, "assets/shaders/triangle.vert.spv");
    fs = loadShader(device, "assets/shaders/triangle.frag.spv");

    layout = createPipelineLayout(device);
    pipeline = createPipeline(device, layout, vs, fs, renderPass);

    scratch = createBuffer(
        device,
        physicalDevice.memoryProperties,
        MB(64),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    vb = createBuffer(
        device,
        physicalDevice.memoryProperties,
        MB(64),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    ib = createBuffer(
        device,
        physicalDevice.memoryProperties,
        MB(64),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    uploadDataToBuffer(device, queue, cmdPool, cmdBuffer, scratch, vb, vertices, sizeof(vertices));
    uploadDataToBuffer(device, queue, cmdPool, cmdBuffer, scratch, ib, indices, sizeof(indices));
}

void beginFrame() {
    swapchain = syncSwapchain(device, physicalDevice.handle, surface, queueFamilyIndex, format, renderPass, swapchain);

    VK_CHECK(vkAcquireNextImageKHR(device, swapchain.handle, UINT64_MAX, acquireSemaphore, VK_NULL_HANDLE, &imageIndex));

    VK_CHECK(vkResetCommandPool(device, cmdPool, 0));

    VK_CHECK(vkBeginCommandBuffer(
        cmdBuffer,
        &(VkCommandBufferBeginInfo) {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        }
    ));

    vkCmdSetViewport(
        cmdBuffer,
        0,
        1,
        &(VkViewport) {
            .x = 0,
            .y = swapchain.height,
            .width = swapchain.width,
            .height = -(f32)swapchain.height,
            .minDepth = 0,
            .maxDepth = 1,
        }
    );

    vkCmdSetScissor(
        cmdBuffer,
        0,
        1,
        &(VkRect2D) {
            .offset = {.x = 0,                   .y = 0                    },
            .extent = {.width = swapchain.width, .height = swapchain.height},
    }
    );

    pipelineImageBarrier(
        cmdBuffer,
        swapchain.images[imageIndex],
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    );
}

void endFrame() {
    pipelineImageBarrier(
        cmdBuffer,
        swapchain.images[imageIndex],
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        0,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    );

    VK_CHECK(vkEndCommandBuffer(cmdBuffer));

    VK_CHECK(vkQueueSubmit(
        queue,
        1,
        &(VkSubmitInfo) {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &acquireSemaphore,
            .pWaitDstStageMask = (VkPipelineStageFlags[]) {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
            .commandBufferCount = 1,
            .pCommandBuffers = &cmdBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &releaseSemaphore,
        },
        VK_NULL_HANDLE
    ));

    VK_CHECK(vkQueuePresentKHR(
        queue,
        &(VkPresentInfoKHR) {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &releaseSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &swapchain.handle,
            .pImageIndices = &imageIndex,
        }
    ));

    VK_CHECK(vkDeviceWaitIdle(device));
}

void destroyRenderer() {
    VK_CHECK(vkDeviceWaitIdle(device));

    destroyBuffer(device, ib);
    destroyBuffer(device, vb);
    destroyBuffer(device, scratch);
    vkDestroyPipelineLayout(device, layout, 0);
    vkDestroyPipeline(device, pipeline, 0);
    vkDestroyShaderModule(device, fs, 0);
    vkDestroyShaderModule(device, vs, 0);
    vkDestroySemaphore(device, releaseSemaphore, 0);
    vkDestroySemaphore(device, acquireSemaphore, 0);
    vkDestroyCommandPool(device, cmdPool, 0);
    destroySwapchain(device, swapchain);
    vkDestroyRenderPass(device, renderPass, 0);
    vkDestroyDevice(device, 0);
    vkDestroySurfaceKHR(instance, surface, 0);
    vkDestroyInstance(instance, 0);
}

#endif /* __RENDERER__ */
