#ifndef __RENDERER__
#define __RENDERER__

#include "instance.h"
#include "device.h"
#include "shader.h"
#include "buffer.h"
#include "swapchain.h"

extern VkInstance instance;
extern VkSurfaceKHR surface;
extern PhysicalDevice physicalDevice;
extern VkDevice device;
extern u32 queueFamilyIndex;
extern VkQueue queue;

extern i32 width, height;
extern VkFormat format;

extern VkRenderPass renderPass;
extern Swapchain swapchain;
extern VkCommandPool cmdPool;
extern VkCommandBuffer cmdBuffer;
extern VkSemaphore acquireSemaphore;
extern VkSemaphore releaseSemaphore;

extern VkShaderModule vs;
extern VkShaderModule fs;

extern VkPipelineLayout layout;
extern VkPipeline pipeline;

extern Buffer scratch;
extern Buffer vb;
extern Buffer ib;

extern u32 imageIndex;

void createRenderer(GLFWwindow* window);

void beginFrame();

void endFrame();

void destroyRenderer();

#endif /* __RENDERER__ */
