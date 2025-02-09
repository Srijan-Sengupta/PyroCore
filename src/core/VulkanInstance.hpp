//
// Created by srijan on 2/9/25.
//

#ifndef VULKANINSTANCE_HPP
#define VULKANINSTANCE_HPP

#include <vulkan/vulkan.h>

#include "../window/PyroWindow.hpp"

namespace pyro {
#ifdef PYRO_DEBUG
    const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
#endif

    class VulkanInstance {
    public:
        VulkanInstance(PyroWindow *window);

        ~VulkanInstance();

#ifdef PYRO_DEBUG
        bool checkValidationLayerSupport();
#endif

    private:
        VkInstance instance{};
        VkDebugUtilsMessengerEXT debug_utils_messenger{};
#ifdef PYRO_DEBUG
        static const char *messageTypeToString(VkDebugUtilsMessageTypeFlagsEXT messageType);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData);
    };
#endif
} // pyro

#endif //VULKANINSTANCE_HPP
