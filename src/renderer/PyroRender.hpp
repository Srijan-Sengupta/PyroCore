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
        PyroPipeline pyroPipeline;

        PyroRender();

        ~PyroRender();

        void run();

        void recreate_swapchain();

    private:
        void draw_frame();

        int current_frame = 0;
    };
} // namespace pyro


#endif // PYRORENDER_HPP
