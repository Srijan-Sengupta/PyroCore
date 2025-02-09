#include <iostream>
#include "logger/Logger.hpp"
#include<format>
int main() {
    std::cout << "Hello, World!" << std::endl;
#ifdef PYRO_DEBUG
    pyro::Logger::getInstance().setLogLevel(pyro::LogLevel::DEBUG);
    pyro::Logger::getInstance().enableFileLogging("pyro.log");
#endif
    LOG(pyro::LogLevel::INFO, "Application started.");
    LOG(pyro::LogLevel::DEBUG, "This debug log will only appear if DEBUG is enabled.");
    LOG(pyro::LogLevel::WARNING, "This is a warning message.");
    LOG(pyro::LogLevel::ERROR, "An error occurred!");
    ASSERT(1 == 1);
    return 0;
}
