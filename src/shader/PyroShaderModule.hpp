//
// Created by srijan on 2/12/25.
//

#ifndef PYROSHADERMODULE_HPP
#define PYROSHADERMODULE_HPP
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "../core/VulkanDevice.hpp"

namespace pyro {
    enum PyroShaderModuleType {
        PYRO_VERTEX,
        PYRO_FRAGMENT,
        PYRO_GEOMETRY,
        PYRO_TESS,
    };
    class PyroShaderModule {
    public:
        PyroShaderModule(VulkanDevice *device, const std::vector<char> &code, PyroShaderModuleType type);
        PyroShaderModule(VulkanDevice *device, const std::string &path, PyroShaderModuleType type);
        ~PyroShaderModule();
        VkShaderModule &getShaderModule() {return shader_module;};
        PyroShaderModuleType get_type() const { return type; }

    private:
        VkShaderModule shader_module;
        PyroShaderModuleType type;
        VulkanDevice *device_;
    };

} // namespace pyro

#endif // PYROSHADERMODULE_HPP
