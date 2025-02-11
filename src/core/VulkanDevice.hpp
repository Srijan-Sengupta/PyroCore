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
    /*enum class RequiredProperties {
        None,
    };
    enum class RequiredFeatures {
        None,
    };*/

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphics_family_index;
        std::optional<uint32_t> present_family_index;
        bool isComplete() {return present_family_index.has_value() && graphics_family_index.has_value();}
    };
    class VulkanDevice {
    public:
        VulkanDevice(VulkanInstance *instance, PyroWindow *window,int gpu_index = 0);
        ~VulkanDevice();

        std::string get_physical_device_name(VkPhysicalDevice *device);

        VkPhysicalDevice get_physical_device() const { return physicalDevice; }
        QueueFamilyIndices get_indices() const { return indices; }
        VkQueue get_graphics_queue() const { return graphicsQueue; }
        VkQueue get_present_queue() const { return presentQueue; }
        VkDevice get_logical_device() const { return logicalDevice; }
        VkSurfaceKHR get_surface() const { return surface; }
        VkCommandPool get_command_pool() const { return commandPool; }

        std::multimap<int, VkPhysicalDevice, std::greater<int>> listPhysicalDevices();
        int rateDevice(const VkPhysicalDevice *device);
        QueueFamilyIndices findQueueFamilyIndex(const VkPhysicalDevice *device);

    private:
        VulkanInstance *instance;
        VkPhysicalDevice physicalDevice;
        QueueFamilyIndices indices;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkDevice logicalDevice;
        VkSurfaceKHR surface;
        VkCommandPool commandPool;
    };

} // namespace pyro

#endif // VULKANDEVICE_HPP
