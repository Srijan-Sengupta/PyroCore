//
// Created by srijan on 2/10/25.
//

#include "VulkanDevice.hpp"

#include <vector>

#include "../utils/Logger.hpp"

namespace pyro {
    VulkanDevice::VulkanDevice(VkInstance *instance, int index) : instance(instance) {
        std::multimap devices(listPhysicalDevices());
        ASSERT_EQUAL(index >= 0 && index < static_cast<int>(devices.size()), true, "Device index out of range.")
        if (index >= 0 && index < static_cast<int>(devices.size())) {
            auto it = devices.begin();
            std::advance(it, index);
            device = &it->second;
        }else {
            device = &devices.begin()->second;
        }
    }
    std::multimap<int, VkPhysicalDevice> VulkanDevice::listPhysicalDevices() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);
        ASSERT_EQUAL(deviceCount == 0, false, "No Supported GPUs. Please Install Vulkan.")
        std::vector<VkPhysicalDevice> all_devices(deviceCount);
        vkEnumeratePhysicalDevices(*instance, &deviceCount, all_devices.data());
        std::multimap<int, VkPhysicalDevice> devices;
        for (auto &device: all_devices) {
            int score{this->rateDevice(&device)};
            devices.insert(std::make_pair(score, device));
        }
        return devices;
    }
    int VulkanDevice::rateDevice(const VkPhysicalDevice *device) {
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceProperties(*device, &properties);
        vkGetPhysicalDeviceFeatures(*device, &features);
        if (!features.geometryShader)
            return -1;
        int score = 0;
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            score += 1000;
        score += properties.limits.maxImageDimension2D;
        return score;
    }
} // namespace pyro
