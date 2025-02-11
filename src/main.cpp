#include <format>
#include <iostream>

#include "core/VulkanDevice.hpp"
#include "utils/Logger.hpp"

#include "core/VulkanInstance.hpp"
#include "window/PyroWindow.hpp"

int main() {
#ifdef PYRO_DEBUG
    pyro::Logger::getInstance().setLogLevel(pyro::LogLevel::DEBUG);
    pyro::Logger::getInstance().enableFileLogging("pyro.log");
#endif
    LOG(pyro::LogLevel::INFO, "Application started.");
    pyro::PyroWindow window{600, 500, "Pyro Core", pyro::WindowOptions::WINDOW_NOT_RESIZABLE};
    pyro::VulkanInstance instance{&window};
    pyro::VulkanDevice device{&instance, &window};
    while (!window.should_close()) {
        window.poll_events();
    }

    return 0;
}
