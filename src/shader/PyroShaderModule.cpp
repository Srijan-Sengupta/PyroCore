//
// Created by srijan on 2/12/25.
//

#include "PyroShaderModule.hpp"

#include "../utils/Logger.hpp"
#include "../utils/ShaderLoader.hpp"

namespace pyro {
    PyroShaderModule::PyroShaderModule(VulkanDevice *device, const std::vector<char> &code,
                                       const PyroShaderModuleType type) : type(type), device_(device) {
        VkShaderModuleCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = code.size();
        info.pCode = reinterpret_cast<const uint32_t *>(code.data());
        LOG(LogLevel::DEBUG, "Creating shader module of size {}", code.size());
        ASSERT_EQUAL(vkCreateShaderModule(device->get_logical_device(), &info, nullptr, &shader_module), VK_SUCCESS,
                     "Failed to create shader module");
    }
    PyroShaderModule::PyroShaderModule(VulkanDevice *device, const std::string &path, const PyroShaderModuleType type) :
        PyroShaderModule(device, ShaderLoader::loadSPV(path), type) {}
    PyroShaderModule::~PyroShaderModule() {
        vkDestroyShaderModule(device_->get_logical_device(), shader_module, nullptr);
    }


} // namespace pyro
