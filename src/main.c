#include "renderer.h"

int main() {
    assert(glfwInit());

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(960, 540, "Vulkan Renderer", 0, 0);
    assert(window);

    createRenderer(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        beginFrame();

        // begin render pass
        VkClearColorValue clearColor = {
            .float32 = {0.1f, 0.2f, 0.2f, 0.0f}
        };
        VkRect2D renderArea = {
            .offset = {.x = 0,                   .y = 0                    },
            .extent = {.width = swapchain.width, .height = swapchain.height},
        };
        VkRenderPassBeginInfo renderPassBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderPass,
            .framebuffer = swapchain.framebuffers[imageIndex],
            .renderArea = renderArea,
            .clearValueCount = 1,
            .pClearValues = &(VkClearValue) {clearColor},
        };

        vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vb.handle, (VkDeviceSize[]) {0});
        vkCmdBindIndexBuffer(cmdBuffer, ib.handle, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(cmdBuffer, ARRAY_SIZE(indices), 1, 0, 0, 0);

        vkCmdEndRenderPass(cmdBuffer);

        endFrame();
    }

    destroyRenderer();

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}
