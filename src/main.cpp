#include <format>
#include <iostream>

#include "core/VulkanDevice.hpp"
#include "utils/Logger.hpp"

#include "core/VulkanInstance.hpp"
#include "renderer/PyroRender.hpp"
#include "renderer/Pyropipeline.hpp"
#include "window/PyroWindow.hpp"

int main() {
#ifdef PYRO_DEBUG
    pyro::Logger::getInstance().setLogLevel(pyro::LogLevel::INFO);
    pyro::Logger::getInstance().enableFileLogging("pyro.log");
#endif
    LOG(pyro::LogLevel::INFO, "Application started.");
    pyro::PyroRender render;
    render.run();
    return 0;
}
