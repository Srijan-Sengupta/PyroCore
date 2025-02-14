//
// Created by srijan on 2/14/25.
//

#ifndef PYRORENDER_HPP
#define PYRORENDER_HPP

#include "../core/VulkanDevice.hpp"
#include "../core/VulkanInstance.hpp"
#include "../window/PyroWindow.hpp"
#include "PyroRender.hpp"
#include "Pyropipeline.hpp"

namespace pyro {
    class PyroRender {
    public:
        PyroWindow window;
        VulkanInstance instance;
        VulkanDevice device;
        Pyropipeline pyroPipeline;
        PyroRender();

        void run();

    private:
        void draw_frame();
    };
} // namespace pyro


#endif // PYRORENDER_HPP
