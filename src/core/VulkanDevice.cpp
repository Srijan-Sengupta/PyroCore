//
// Created by srijan on 2/10/25.
//

#include "VulkanDevice.hpp"

#include <queue>
#include <set>
#include <vector>

#include "../utils/Logger.hpp"
#include "../window/PyroWindow.hpp"
#include "VulkanInstance.hpp"

namespace pyro {
    VulkanDevice::VulkanDevice(VulkanInstance *instance, PyroWindow *window, int gpu_index) : instance(instance) {

        surface = window->create_surface((instance->getInstance()));
        std::multimap devices(listPhysicalDevices());
        ASSERT_EQUAL(gpu_index >= 0 && gpu_index < static_cast<int>(devices.size()), true,
                     "Device gpu_index out of range.")
        if (gpu_index >= 0 && gpu_index < static_cast<int>(devices.size())) {
            auto it = devices.begin();
            std::advance(it, gpu_index);
            physicalDevice = it->second;
        } else {
            physicalDevice = devices.begin()->second;
        }
        std::string device_name(get_physical_device_name(&physicalDevice));
        LOG(LogLevel::INFO, "Created Vulkan device: {}", device_name);
        // Creating Logical Device
        indices = findQueueFamilyIndex(&physicalDevice);
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set uniqueQueueFamilyIndices = {indices.graphics_family_index.value(),
                                             indices.present_family_index.value()};

        float queuePriority = 1.0f;
        for (const auto &queue_family: uniqueQueueFamilyIndices) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queue_family;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.enabledExtensionCount = 0;
#ifdef PYRO_DEBUG
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(instance->validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = instance->validationLayers.data();
#endif
        ASSERT_EQUAL(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice), VK_SUCCESS,
                     "Failed to create logical device.");
        vkGetDeviceQueue(logicalDevice, indices.graphics_family_index.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(logicalDevice, indices.present_family_index.value(), 0, &presentQueue);
        ASSERT_EQUAL(graphicsQueue == nullptr, false, "Failed to find graphics queue on this device")
        ASSERT_EQUAL(presentQueue == nullptr, false, "Failed to find present queue on this device")
    }
    VulkanDevice::~VulkanDevice() {
        vkDestroyDevice(logicalDevice, nullptr);
        vkDestroySurfaceKHR(*(instance->getInstance()), surface, nullptr);
    }
    std::string VulkanDevice::get_physical_device_name(VkPhysicalDevice* device) {
        VkPhysicalDeviceProperties prop{};
        vkGetPhysicalDeviceProperties(*device, &prop);

        ASSERT_EQUAL(strlen(prop.deviceName) > 0, true, "Failed to get physical device properties");

        return prop.deviceName;
    }

    QueueFamilyIndices VulkanDevice::findQueueFamilyIndex(const VkPhysicalDevice *device) {
        QueueFamilyIndices q_indices;
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(*device, &queue_family_count, nullptr);
        ASSERT_EQUAL(queue_family_count != 0, true, "No queue families found.")
        std::vector<VkQueueFamilyProperties> queueFamilies(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(*device, &queue_family_count, queueFamilies.data());
        int i = 0;
        for (auto &qf: queueFamilies) {
            if (qf.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                q_indices.graphics_family_index = i;
            }
            VkBool32 presentSupported = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(*device, i, surface, &presentSupported);
            if (presentSupported) {
                q_indices.present_family_index = i;
            }

            if (q_indices.isComplete())
                break;
            i++;
        }
        return q_indices;
    }
    std::multimap<int, VkPhysicalDevice, std::greater<int>> VulkanDevice::listPhysicalDevices() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(*instance->getInstance(), &deviceCount, nullptr);
        ASSERT_EQUAL(deviceCount == 0, false, "No Supported GPUs. Please Install Vulkan.")
        std::vector<VkPhysicalDevice> all_devices(deviceCount);
        vkEnumeratePhysicalDevices(*instance->getInstance(), &deviceCount, all_devices.data());
        std::multimap<int, VkPhysicalDevice, std::greater<int>> devices;
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
        if (!findQueueFamilyIndex(device).isComplete())
            return -1;
        int score = 0;
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            score += 1000;
        score += properties.limits.maxImageDimension2D;
        return score;
    }
} // namespace pyro
