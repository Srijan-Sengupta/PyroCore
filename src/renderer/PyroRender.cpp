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
    PyroRender::PyroRender() : window(600, 500, "PyroCore", WINDOW_NOT_RESIZABLE), instance(&window),
                               device(&instance, &window), pyroPipeline(&device) {
        auto handle_resize = [this](uint32_t, uint32_t) {
            LOG(LogLevel::INFO, "Resizing window callback");
            recreate_swapchain();
        };
        window.set_resize_callback(handle_resize);
    }

    PyroRender::~PyroRender() {
        LOG(LogLevel::DEBUG, "Destroying Vulkan Renderer");
    }

    void PyroRender::run() {
        while (!window.should_close()) {
            window.poll_events();
            draw_frame();
        }
        vkDeviceWaitIdle(device.get_logical_device());
    }

    void PyroRender::recreate_swapchain() {
        LOG(LogLevel::WARNING, "Swap chain too old recreating...");
        vkDeviceWaitIdle(device.get_logical_device());
        device.destroy_swap_chain();
        pyroPipeline.destroy_frame_buffers();
        device.create_swap_chain(&window);
        device.create_image_views();
        pyroPipeline.create_frame_buffers();
    }

    void PyroRender::draw_frame() {
        vkWaitForFences(device.get_logical_device(), 1, &device.get_inflight_fence()[current_frame], VK_TRUE,
                        UINT64_MAX);
        uint32_t image_index;
        const VkResult next_image_result = vkAcquireNextImageKHR(device.get_logical_device(), device.get_swap_chain(),
                                                           UINT64_MAX,
                                                           device.get_image_available_semaphore()[current_frame],
                                                           VK_NULL_HANDLE, &image_index);
        if (next_image_result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreate_swapchain();
            return;
        } else if (next_image_result != VK_SUCCESS && next_image_result != VK_SUBOPTIMAL_KHR) {
            LOG(LogLevel::ERROR, "Failed to acquire image from swap chain");
        }
        vkResetFences(device.get_logical_device(), 1, &device.get_inflight_fence()[current_frame]);
        vkResetCommandBuffer(device.get_command_buffer()[current_frame], 0);
        VulkanDevice::record_command_buffer(device.get_command_buffer()[current_frame], image_index,
                                            pyroPipeline.get_render_pass(),
                                            pyroPipeline.get_pipeline(), pyroPipeline.get_swap_chain_framebuffers(),
                                            device.get_swap_chain_extent());
        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkSemaphore wait_semaphores[] = {device.get_image_available_semaphore()[current_frame]};
        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &device.get_command_buffer()[current_frame];

        VkSemaphore signal_semaphores[] = {device.get_render_finished_semaphore()[current_frame]};
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        VkResult present_result = vkQueueSubmit(device.get_graphics_queue(), 1, &submit_info,
                                                device.get_inflight_fence()[current_frame]);
        if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR) {
            recreate_swapchain();
        } else if (present_result != VK_SUCCESS) {
            LOG(LogLevel::ERROR, "Failed to acquire image from swap chain");
        }
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
        current_frame = (current_frame + 1) % MAX_FRAME_ON_FLIGHT;
    }
} // namespace pyro
