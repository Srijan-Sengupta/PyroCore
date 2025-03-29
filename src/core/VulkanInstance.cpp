//
// Created by srijan on 2/9/25.
//

#include "VulkanInstance.hpp"
#include "../utils/Logger.hpp"

namespace pyro {
    VulkanInstance::VulkanInstance(PyroWindow *window) {
        VkApplicationInfo app_info = {};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.apiVersion = VK_API_VERSION_1_3;
        app_info.pApplicationName = "pyro";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "pyro core";
        app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);

        uint32_t extension_count = 0;
        char const *const *extensions = window->get_instance_extensions(&extension_count);

        VkInstanceCreateInfo instance_create_info = {};
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pApplicationInfo = &app_info;
        instance_create_info.enabledExtensionCount = extension_count;
        instance_create_info.ppEnabledExtensionNames = extensions;
        instance_create_info.enabledLayerCount = 0;
#ifdef PYRO_DEBUG
        ASSERT_EQUAL(checkValidationLayerSupport(), true, "Failed to find Validation Layer");
        std::vector extensions_vector(extensions, extensions + extension_count);
        extensions_vector.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        instance_create_info.enabledExtensionCount = extensions_vector.size();
        instance_create_info.ppEnabledExtensionNames = extensions_vector.data();
        instance_create_info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instance_create_info.ppEnabledLayerNames = validationLayers.data();

        std::string ext;
        for (const auto &i: extensions_vector) {
            ext += +i;
            ext += ", ";
        }
        LOG(LogLevel::DEBUG, "{} Extensions: {}", extensions_vector.size(), ext);

        VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info = {};
        debug_utils_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_utils_messenger_create_info.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_utils_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_utils_messenger_create_info.pfnUserCallback = debugCallback;
        instance_create_info.pNext = reinterpret_cast<VkDebugUtilsMessengerEXT *>(&debug_utils_messenger_create_info);
#endif
        ASSERT_EQUAL(vkCreateInstance(&instance_create_info, nullptr, &instance), VK_SUCCESS,
               "Failed to create Vulkan Instance")
        LOG(LogLevel::INFO, "Created Vulkan Instance");
        LOG(LogLevel::DEBUG, "Enabled extensions: {}", extension_count);

#ifdef PYRO_DEBUG
        auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(instance, ("vkCreateDebugUtilsMessengerEXT")));
        ASSERT_EQUAL(!func, false, "Failed to find vkCreateDebugUtilsMessengerEXT function")
        ASSERT_EQUAL(func(instance, &debug_utils_messenger_create_info, nullptr, &debug_utils_messenger), VK_SUCCESS,
               "Failed to call vkCreateDebugUtilsMessengerEXT")
#endif
    }

    VulkanInstance::~VulkanInstance() {
#ifdef PYRO_DEBUG
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        ASSERT_EQUAL(!func, false, "Failed to find vkDestroyDebugUtilsMessengerEXT function")
        func(instance, debug_utils_messenger, nullptr);
#endif
        LOG(LogLevel::INFO, "Destroyed Vulkan Instance");
        vkDestroyInstance(instance, nullptr);
    }
#ifdef PYRO_DEBUG
    bool VulkanInstance::checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        for (const char *layerName: validationLayers) {
            bool layerFound = false;

            for (const auto &layerProperties: availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }
        return true;
    }

    VkBool32 VulkanInstance::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                           VkDebugUtilsMessageTypeFlagsEXT messageType,
                                           const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
        const char *msg_type = messageTypeToString(messageType);
        // Log based on severity level
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
            LOG(LogLevel::DEBUG, "Vulkan verbose message: {} {}", msg_type, pCallbackData->pMessage);
        }
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
            LOG(LogLevel::INFO, "Vulkan info message: {} {}", msg_type, pCallbackData->pMessage);
        }
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            LOG(LogLevel::WARNING, "Vulkan warning: {} {}", msg_type, pCallbackData->pMessage);
        }
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            LOG(LogLevel::ERROR, "Vulkan error: {} {}", msg_type, pCallbackData->pMessage);
        }

        // Returning VK_FALSE will let Vulkan continue processing; VK_TRUE would stop the operation.
        return VK_FALSE;
    }

    const char *VulkanInstance::messageTypeToString(VkDebugUtilsMessageTypeFlagsEXT messageType) {
        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
            return "General";
        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
            return "Validation";
        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
            return "Performance";
        return "Unknown";
    }
#endif
} // namespace pyro
