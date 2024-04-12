#include "renderer.h"

int main() {
    createRenderer();

    while (!glfwWindowShouldClose(renderer.window)) {
        glfwPollEvents();

        renderer.swapchain = syncSwapchain(
            renderer.device,
            renderer.physicalDevice.handle,
            renderer.surface,
            renderer.queueFamilyIndex,
            renderer.format,
            renderer.renderPass,
            renderer.swapchain
        );

        u32 imageIndex = 0;
        VK_CHECK(vkAcquireNextImageKHR(
            renderer.device, renderer.swapchain.handle, ~0ull, renderer.acquireSemaphore, VK_NULL_HANDLE, &imageIndex
        ));

        // begin command buffer
        VK_CHECK(vkResetCommandPool(renderer.device, renderer.commandPool, 0));

        VkCommandBufferBeginInfo cmdBufferBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        VK_CHECK(vkBeginCommandBuffer(renderer.cmdBuffer, &cmdBufferBeginInfo));

        pipelineImageBarrier(
            renderer.cmdBuffer,
            renderer.swapchain.images[imageIndex],
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        );

        // begin render pass
        VkClearColorValue clearColor = {
            .float32 = {0.1f, 0.2f, 0.2f, 0.0f}
        };
        VkClearValue clearValue = {clearColor};
        VkRect2D renderArea = {
            .offset = {.x = 0,                            .y = 0                             },
            .extent = {.width = renderer.swapchain.width, .height = renderer.swapchain.height},
        };
        VkRenderPassBeginInfo renderPassBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderer.renderPass,
            .framebuffer = renderer.swapchain.framebuffers[imageIndex],
            .renderArea = renderArea,
            .clearValueCount = 1,
            .pClearValues = &clearValue,
        };

        vkCmdBeginRenderPass(renderer.cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport = {
            .x = 0,
            .y = renderer.swapchain.height,
            .width = renderer.swapchain.width,
            .height = -(f32)renderer.swapchain.height,
            .minDepth = 0,
            .maxDepth = 1,
        };
        vkCmdSetViewport(renderer.cmdBuffer, 0, 1, &viewport);

        VkRect2D scissor = {
            .offset = {.x = 0,                            .y = 0                             },
            .extent = {.width = renderer.swapchain.width, .height = renderer.swapchain.height},
        };
        vkCmdSetScissor(renderer.cmdBuffer, 0, 1, &scissor);

        vkCmdBindPipeline(renderer.cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderer.pipeline);

        vkCmdBindVertexBuffers(renderer.cmdBuffer, 0, 1, &renderer.vb.handle, (VkDeviceSize[]) {0});
        vkCmdBindIndexBuffer(renderer.cmdBuffer, renderer.ib.handle, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(renderer.cmdBuffer, ARRAY_SIZE(indices), 1, 0, 0, 0);

        vkCmdEndRenderPass(renderer.cmdBuffer);

        pipelineImageBarrier(
            renderer.cmdBuffer,
            renderer.swapchain.images[imageIndex],
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            0,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        );

        VK_CHECK(vkEndCommandBuffer(renderer.cmdBuffer));

        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &renderer.acquireSemaphore,
            .pWaitDstStageMask = waitStages,
            .commandBufferCount = 1,
            .pCommandBuffers = &renderer.cmdBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &renderer.releaseSemaphore,
        };
        VK_CHECK(vkQueueSubmit(renderer.queue, 1, &submitInfo, VK_NULL_HANDLE));

        VkPresentInfoKHR presentInfo = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &renderer.releaseSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &renderer.swapchain.handle,
            .pImageIndices = &imageIndex,
        };
        VK_CHECK(vkQueuePresentKHR(renderer.queue, &presentInfo));

        VK_CHECK(vkDeviceWaitIdle(renderer.device));
    }

    destroyRenderer();
    return 0;
}
