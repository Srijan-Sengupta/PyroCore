#include <iostream>
#include "logger/Logger.hpp"
#include<format>

#include "window/PyroWindow.hpp"

int main() {
#ifdef PYRO_DEBUG
    pyro::Logger::getInstance().setLogLevel(pyro::LogLevel::DEBUG);
    pyro::Logger::getInstance().enableFileLogging("pyro.log");
#endif
    LOG(pyro::LogLevel::INFO, "Application started.");

    pyro::PyroWindow window{600, 500, "Pyro Core",pyro::WindowOptions::WINDOW_NOT_RESIZABLE};
    while (!window.should_close()) {
        window.poll_events();
    }

    return 0;
}
