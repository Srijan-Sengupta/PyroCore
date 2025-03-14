//
// Created by srijan on 2/7/25.
//

#include "PyroWindow.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <string>
#include <vulkan/vulkan_core.h>

#include "../utils/Logger.hpp"

namespace pyro {
    PyroWindow::PyroWindow(int width, int height, const std::string &title, int options) :
        width(width), height(height) {
        SDL_WindowFlags sdl_options = 0;
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

        sdl_options |= (options & WINDOW_FULLSCREEN) ? SDL_WINDOW_FULLSCREEN : 0;
        sdl_options |= (options & WINDOW_NOT_RESIZABLE) ? 0 : SDL_WINDOW_RESIZABLE;
        sdl_options |= (options & WINDOW_BORDERLESS) ? SDL_WINDOW_BORDERLESS : 0;
        sdl_options |= (options & WINDOW_HIDDEN) ? SDL_WINDOW_HIDDEN : 0;
        sdl_options |= (options & WINDOW_MAXIMIZE) ? SDL_WINDOW_MAXIMIZED : SDL_WINDOW_MINIMIZED;
        sdl_options |= (options & WINDOW_ALWAYS_ON_TOP) ? SDL_WINDOW_ALWAYS_ON_TOP : 0;
        sdl_options |= (options & WINDOW_UNFOCUSED) ? SDL_WINDOW_NOT_FOCUSABLE : SDL_WINDOW_INPUT_FOCUS;

        window = SDL_CreateWindow(title.c_str(), width, height, sdl_options | SDL_WINDOW_VULKAN);
        ASSERT_EQUAL(!window, false, "Failed to create window");
    }

    PyroWindow::~PyroWindow() {
        SDL_DestroyWindow(window);
        SDL_Quit();
        LOG(LogLevel::INFO, "Window destroyed");
    }

    bool PyroWindow::should_close() { return event.type == SDL_EVENT_QUIT; }

    void PyroWindow::poll_events() { SDL_PollEvent(&event); }
    VkExtent2D PyroWindow::get_extent() {
        int width, height;
        SDL_GetWindowSizeInPixels(window, &width, &height);
        VkExtent2D extent = {
        .width = static_cast<uint32_t>(width) ,
        .height = static_cast<uint32_t>(height),
        };
        return extent;
    }

    char const *const *PyroWindow::get_instance_extensions(uint32_t *ext_count) {
        char const *const *extensions;
        extensions = SDL_Vulkan_GetInstanceExtensions(ext_count);
        return extensions;
    }
    VkSurfaceKHR PyroWindow::create_surface(VkInstance *instance) {
        VkSurfaceKHR surface;
        ASSERT_EQUAL(SDL_Vulkan_CreateSurface(window, *instance, nullptr, &surface), true, "Failed to create surface");
        return surface;
    }
} // namespace pyro
