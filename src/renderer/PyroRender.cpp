//
// Created by srijan on 2/14/25.
//

#include "PyroRender.hpp"

#include "../core/VulkanDevice.hpp"
#include "../core/VulkanInstance.hpp"
#include "../utils/Logger.hpp"
#include "../window/PyroWindow.hpp"
#include "Pyropipeline.hpp"

namespace pyro {

    PyroRender::PyroRender() :
        window(600, 500, "PyroCore", WindowOptions::WINDOW_NOT_RESIZABLE), instance(&window),
        device(&instance, &window), pyroPipeline(&device) {}

    void PyroRender::run() {
        while (!window.should_close()) {
            window.poll_events();
            draw_frame();
        }
        vkDeviceWaitIdle(device.get_logical_device());
    }
    void PyroRender::draw_frame() {
        vkWaitForFences(device.get_logical_device(), 1, device.get_inflight_fence(), VK_TRUE, UINT64_MAX);
        vkResetFences(device.get_logical_device(), 1, device.get_inflight_fence());
        uint32_t image_index;
        ASSERT_EQUAL(vkAcquireNextImageKHR(device.get_logical_device(), device.get_swap_chain(), UINT64_MAX,
                                           device.get_image_available_semaphore(), VK_NULL_HANDLE, &image_index),
                     VK_SUCCESS, "Failed to Acquire next image")
        vkResetCommandBuffer(*device.get_command_buffer(), 0);
        device.record_command_buffer(*device.get_command_buffer(), image_index, pyroPipeline.get_render_pass(),
                                     pyroPipeline.get_pipeline(), pyroPipeline.get_swap_chain_framebuffers());
        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkSemaphore wait_semaphores[] = {device.get_image_available_semaphore()};
        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = device.get_command_buffer();

        VkSemaphore signal_semaphores[] = {device.get_render_finished_semaphore()};
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        ASSERT_EQUAL(vkQueueSubmit(device.get_graphics_queue(), 1, &submit_info, *(device.get_inflight_fence())),
                     VK_SUCCESS, "Failed to submit command buffer")
        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swapchains[] = {device.get_swap_chain()};
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapchains;
        present_info.pImageIndices = &image_index;
        present_info.pResults = nullptr;
        vkQueuePresentKHR(device.get_present_queue(), &present_info);
    }
} // namespace pyro
