//
// Created by srijan on 2/10/25.
//

#ifndef VULKANDEVICE_HPP
#define VULKANDEVICE_HPP

#include <map>
#include <vulkan/vulkan.h>

namespace pyro {
    /*enum class RequiredProperties {
        None,
    };
    enum class RequiredFeatures {
        None,
    };*/

    class VulkanDevice {
    public:
        VulkanDevice(VkInstance *instance, int index=-1);
        ~VulkanDevice();

        std::multimap<int, VkPhysicalDevice> listPhysicalDevices();
        int rateDevice(const VkPhysicalDevice * device);

    private:
        VkInstance *instance;
        VkPhysicalDevice *device;
    };

} // namespace pyro

#endif // VULKANDEVICE_HPP
