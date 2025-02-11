//
// Created by srijan on 2/9/25.
//

#ifndef VULKANINSTANCE_HPP
#define VULKANINSTANCE_HPP

#include <vulkan/vulkan.h>

#include "../window/PyroWindow.hpp"

namespace pyro {
    class VulkanInstance {
    public:
#ifdef PYRO_DEBUG
        const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
#endif
        VulkanInstance(PyroWindow *window);

        ~VulkanInstance();
        VkInstance *getInstance() { return &instance; }

#ifdef PYRO_DEBUG
        bool checkValidationLayerSupport();
#endif

    private:
        VkInstance instance{};
#ifdef PYRO_DEBUG
        VkDebugUtilsMessengerEXT debug_utils_messenger{};

        static const char *messageTypeToString(VkDebugUtilsMessageTypeFlagsEXT messageType);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                            void *pUserData);
    };
#endif
} // namespace pyro

#endif // VULKANINSTANCE_HPP
