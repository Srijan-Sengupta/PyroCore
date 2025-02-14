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
        const std::set uniqueQueueFamilyIndices = {indices.graphics_family_index.value(),
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
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
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
        SwapChainSupportDetails swap_support = {};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swap_support.capabilities);
        uint32_t format_count = 0;
        uint32_t present_mode_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &format_count, nullptr);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &present_mode_count, nullptr);
        ASSERT_EQUAL(format_count != 0, true, "Failed to get surface formats on this device")
        ASSERT_EQUAL(present_mode_count != 0, true, "Failed to get surface presentation modes on this device")
        swap_support.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &format_count, swap_support.formats.data());
        swap_support.presentModes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &present_mode_count,
                                                  swap_support.presentModes.data());


        // Creating Swap chain.
        VkSurfaceFormatKHR surface_format = chooseSwapSurfaceFormat(swap_support.formats);
        VkPresentModeKHR present_mode = chooseSwapPresentMode(swap_support.presentModes);
        VkExtent2D swap_extent = chooseSwapExtent(swap_support.capabilities, window);
        uint32_t image_count = swap_support.capabilities.minImageCount + 1;
        if (swap_support.capabilities.maxImageCount > 0 && image_count > swap_support.capabilities.maxImageCount) {
            image_count = swap_support.capabilities.maxImageCount;
        }
        VkSwapchainCreateInfoKHR swap_create_info = {};
        swap_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swap_create_info.surface = surface;
        swap_create_info.minImageCount = image_count;
        swap_create_info.imageFormat = surface_format.format;
        swap_create_info.imageColorSpace = surface_format.colorSpace;
        swap_create_info.imageExtent = swap_extent;
        swap_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swap_create_info.imageArrayLayers = 1;
        uint32_t queueFamilyIndex[] = {indices.graphics_family_index.value(), indices.present_family_index.value()};
        if (indices.graphics_family_index.value() == indices.present_family_index.value()) {
            swap_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swap_create_info.queueFamilyIndexCount = 0;
            swap_create_info.pQueueFamilyIndices = nullptr;
        } else {
            swap_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swap_create_info.queueFamilyIndexCount = 2;
            swap_create_info.pQueueFamilyIndices = queueFamilyIndex;
        }
        swap_create_info.preTransform = swap_support.capabilities.currentTransform;
        swap_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swap_create_info.presentMode = present_mode;
        swap_create_info.clipped = VK_TRUE;
        swap_create_info.oldSwapchain = VK_NULL_HANDLE;
        ASSERT_EQUAL(vkCreateSwapchainKHR(logicalDevice, &swap_create_info, nullptr, &swapChain), VK_SUCCESS,
                     "Failed to create swap chain")
        swapChainExtent = swap_extent;
        swapChainImageFormat = surface_format.format;

        uint32_t swapChainImageCount;
        vkGetSwapchainImagesKHR(logicalDevice, swapChain, &swapChainImageCount, nullptr);
        swapChainImages.resize(swapChainImageCount);
        vkGetSwapchainImagesKHR(logicalDevice, swapChain, &swapChainImageCount, swapChainImages.data());

        // Creating Image Views
        swapChainImageViews.resize(swapChainImageCount);
        for (uint32_t i = 0; i < swapChainImageCount; i++) {
            VkImageViewCreateInfo view_create_info = {};
            view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            view_create_info.image = swapChainImages[i];
            view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            view_create_info.format = swapChainImageFormat;
            view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            view_create_info.subresourceRange.baseMipLevel = 0;
            view_create_info.subresourceRange.levelCount = 1;
            view_create_info.subresourceRange.baseArrayLayer = 0;
            view_create_info.subresourceRange.layerCount = 1;
            ASSERT_EQUAL(vkCreateImageView(logicalDevice, &view_create_info, nullptr, &swapChainImageViews[i]),
                         VK_SUCCESS, "Failed to create image views")
            // Create Command pool
            VkCommandPoolCreateInfo command_pool_create_info = {};
            command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            command_pool_create_info.queueFamilyIndex = indices.graphics_family_index.value();
            ASSERT_EQUAL(vkCreateCommandPool(logicalDevice, &command_pool_create_info, nullptr, &commandPool),
                         VK_SUCCESS, "Failed to create command pool")

            VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
            command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            command_buffer_allocate_info.commandPool = commandPool;
            command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            command_buffer_allocate_info.commandBufferCount = 1;

            ASSERT_EQUAL(vkAllocateCommandBuffers(logicalDevice, &command_buffer_allocate_info, &commandBuffer),
                         VK_SUCCESS, "Failed to allocate command buffers")

            VkSemaphoreCreateInfo semaphore_create_info = {};
            semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            VkFenceCreateInfo fence_create_info = {};
            fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            ASSERT_EQUAL(vkCreateSemaphore(logicalDevice, &semaphore_create_info, nullptr, &imageAvailableSemaphore),
                         VK_SUCCESS, "Failed to create semaphore")
            ASSERT_EQUAL(vkCreateSemaphore(logicalDevice, &semaphore_create_info, nullptr, &renderFinishedSemaphore),
                         VK_SUCCESS, "Failed to create semaphore")
            ASSERT_EQUAL(vkCreateFence(logicalDevice, &fence_create_info, nullptr, &inflightFence), VK_SUCCESS,
                         "Failed to create fence")
        }
    }
    VulkanDevice::~VulkanDevice() {
        vkDeviceWaitIdle(logicalDevice);
        vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);
        vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
        vkDestroyFence(logicalDevice, inflightFence, nullptr);
        vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
        for (auto imageView: swapChainImageViews) {
            vkDestroyImageView(logicalDevice, imageView, nullptr);
        }
        vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
        vkDestroyDevice(logicalDevice, nullptr);
        vkDestroySurfaceKHR(*instance->getInstance(), surface, nullptr);
    }
    void VulkanDevice::initializeSwapChain(PyroWindow *window) const {}
    VkSurfaceFormatKHR VulkanDevice::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats) {
        for (const auto &format: formats) {
            if (format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB) {
                return format;
            }
        }
        return formats[0];
    }
    VkPresentModeKHR VulkanDevice::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &modes) {
        for (const auto &mode: modes) {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return mode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    VkExtent2D VulkanDevice::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, PyroWindow *window) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        }
        VkExtent2D swap_extent = window->get_extent();
        swap_extent.width =
                std::clamp(swap_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        swap_extent.height =
                std::clamp(swap_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return swap_extent;
    }

    std::string VulkanDevice::get_physical_device_name(const VkPhysicalDevice *device) {
        VkPhysicalDeviceProperties prop{};
        vkGetPhysicalDeviceProperties(*device, &prop);

        ASSERT_EQUAL(strlen(prop.deviceName) > 0, true, "Failed to get physical device properties");

        return prop.deviceName;
    }
    void VulkanDevice::record_command_buffer(const VkCommandBuffer &command_buffer, const uint32_t imageIndex,
                                             const VkRenderPass &renderPass, const VkPipeline graphics_pipeline,
                                             std::vector<VkFramebuffer> swapChainFrameBuffers) {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = 0;
        begin_info.pInheritanceInfo = nullptr;
        ASSERT_EQUAL(vkBeginCommandBuffer(command_buffer, &begin_info), VK_SUCCESS,
                     "Failed to begin recording command buffer")
        VkRenderPassBeginInfo render_pass_begin_info{};
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.renderPass = renderPass;
        render_pass_begin_info.framebuffer = swapChainFrameBuffers[imageIndex];
        render_pass_begin_info.renderArea.offset = {0, 0};
        render_pass_begin_info.renderArea.extent = swapChainExtent;
        const VkClearValue clear_value{0.0f, 0.0f, 0.0f, 1.0f};
        render_pass_begin_info.clearValueCount = 1;
        render_pass_begin_info.pClearValues = &clear_value;
        vkCmdBeginRenderPass(commandBuffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = swapChainExtent.width;
        viewport.height = swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        VkRect2D scissor{};
        scissor.extent = swapChainExtent;
        scissor.offset = {0, 0};
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        vkCmdEndRenderPass(commandBuffer);
        ASSERT_EQUAL(vkEndCommandBuffer(commandBuffer), VK_SUCCESS, "Failed to record command buffer")
    }

    QueueFamilyIndices VulkanDevice::findQueueFamilyIndex(const VkPhysicalDevice *device) const {
        QueueFamilyIndices q_indices;
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(*device, &queue_family_count, nullptr);
        ASSERT_EQUAL(queue_family_count != 0, true, "No queue families found.")
        std::vector<VkQueueFamilyProperties> queueFamilies(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(*device, &queue_family_count, queueFamilies.data());
        int i = 0;
        for (const auto &qf: queueFamilies) {
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
    std::multimap<int, VkPhysicalDevice, std::greater<>> VulkanDevice::listPhysicalDevices() const {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(*instance->getInstance(), &deviceCount, nullptr);
        ASSERT_EQUAL(deviceCount == 0, false, "No Supported GPUs. Please Install Vulkan.")
        std::vector<VkPhysicalDevice> all_devices(deviceCount);
        vkEnumeratePhysicalDevices(*instance->getInstance(), &deviceCount, all_devices.data());
        std::multimap<int, VkPhysicalDevice, std::greater<>> devices;
        for (auto &device: all_devices) {
            int score{this->rateDevice(&device)};
            devices.insert(std::make_pair(score, device));
        }
        return devices;
    }
    int VulkanDevice::rateDevice(const VkPhysicalDevice *device) const {
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        uint32_t extension_count = 0;
        vkEnumerateDeviceExtensionProperties(*device, nullptr, &extension_count, nullptr);
        std::vector<VkExtensionProperties> extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(*device, nullptr, &extension_count, extensions.data());
        vkGetPhysicalDeviceProperties(*device, &properties);
        vkGetPhysicalDeviceFeatures(*device, &features);
        std::set<std::string> required_extensions(deviceExtensions.begin(), deviceExtensions.end());
        for (const auto &[extensionName, specVersion]: extensions) {
            required_extensions.erase(extensionName);
        }
        if (!required_extensions.empty()) {
            return -1;
        }
        if (!features.geometryShader)
            return -1;
        if (!findQueueFamilyIndex(device).isComplete())
            return -1;
        int score = 0;
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            score += 1000;
        score += static_cast<int>(properties.limits.maxImageDimension2D);
        return score;
    }
} // namespace pyro
