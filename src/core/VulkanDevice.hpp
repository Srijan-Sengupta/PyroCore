//
// Created by srijan on 2/10/25.
//

#ifndef VULKANDEVICE_HPP
#define VULKANDEVICE_HPP

#include <map>
#include <optional>
#include <vulkan/vulkan.h>

#include "../window/PyroWindow.hpp"
#include "VulkanInstance.hpp"

namespace pyro {
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphics_family_index;
        std::optional<uint32_t> present_family_index;
        bool isComplete() { return present_family_index.has_value() && graphics_family_index.has_value(); }
    };
    class VulkanDevice {
    public:
        VulkanDevice(VulkanInstance *instance, PyroWindow *window, int gpu_index = 0);
        ~VulkanDevice();

        static std::string get_physical_device_name(const VkPhysicalDevice *device);
        void record_command_buffer(const VkCommandBuffer &command_buffer, uint32_t imageIndex,
                                   const VkRenderPass &renderPass, VkPipeline graphics_pipeline,
                                   std::vector<VkFramebuffer> swapChainFrameBuffers);


        VkPhysicalDevice get_physical_device() const { return physicalDevice; }
        QueueFamilyIndices get_indices() const { return indices; }
        VkQueue get_graphics_queue() const { return graphicsQueue; }
        VkQueue get_present_queue() const { return presentQueue; }
        VkDevice get_logical_device() const { return logicalDevice; }
        VkSurfaceKHR get_surface() const { return surface; }
        VkCommandPool get_command_pool() const { return commandPool; }
        VkExtent2D get_swap_chain_extent() const { return swapChainExtent; }
        VkSwapchainKHR get_swap_chain() const { return swapChain; }
        std::vector<VkImage> get_swap_chain_images() const { return swapChainImages; }
        VkFormat get_swap_chain_image_format() const { return swapChainImageFormat; }
        std::vector<VkImageView> get_swap_chain_image_views() const { return swapChainImageViews; }
        VkSemaphore get_image_available_semaphore() const { return imageAvailableSemaphore; }
        VkSemaphore get_render_finished_semaphore() const { return renderFinishedSemaphore; }
        const VkFence *get_inflight_fence() const { return &inflightFence; }
        const VkCommandBuffer *get_command_buffer() const { return &commandBuffer; }

        std::multimap<int, VkPhysicalDevice, std::greater<>> listPhysicalDevices() const;
        int rateDevice(const VkPhysicalDevice *device) const;
        QueueFamilyIndices findQueueFamilyIndex(const VkPhysicalDevice *device) const;

    private:
        VulkanInstance *instance;
        VkPhysicalDevice physicalDevice;
        QueueFamilyIndices indices;
        VkQueue graphicsQueue{};
        VkQueue presentQueue{};
        VkDevice logicalDevice{};
        VkSurfaceKHR surface;
        VkCommandPool commandPool{};
        VkCommandBuffer commandBuffer{};
        VkSwapchainKHR swapChain{};
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;
        VkSemaphore imageAvailableSemaphore{};
        VkSemaphore renderFinishedSemaphore{};
        VkFence inflightFence{};


        void initializeSwapChain(PyroWindow *window) const;
        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);
        static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &modes);
        static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, PyroWindow *window);


    };

} // namespace pyro

#endif // VULKANDEVICE_HPP
