//
// Created by srijan on 2/7/25.
//

#ifndef PYROWINDOW_HPP
#define PYROWINDOW_HPP


#include <functional>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <string>
#include <vulkan/vulkan_core.h>

namespace pyro {
    enum WindowOptions {
        WINDOW_FULLSCREEN = 1 << 0,
        WINDOW_NOT_RESIZABLE = 1 << 1,
        WINDOW_BORDERLESS = 1 << 2,
        WINDOW_HIDDEN = 1 << 3,
        WINDOW_MAXIMIZE = 1 << 4,
        WINDOW_ALWAYS_ON_TOP = 1 << 5,
        WINDOW_UNFOCUSED = 1 << 6,
    };

    class PyroWindow {
    public:
        PyroWindow(int width, int height, const std::string &title, int options);

        ~PyroWindow();

        bool should_close();

        void poll_events();

        VkExtent2D get_extent();


        char const *const *get_instance_extensions(uint32_t *ext_count);
        VkSurfaceKHR create_surface(VkInstance *instance);
        bool is_resized() const {
            return resized;
        }
        void set_resize_callback(std::function<void(uint32_t w, uint32_t h)> resize_callback) {
            this->resize_callback = resize_callback;
        }

    private:
        SDL_Window *window;
        SDL_Event event;
        uint32_t width;
        uint32_t height;
        bool resized;
        std::function<void(uint32_t w, uint32_t h)> resize_callback;
    };
} // namespace pyro

#endif // PYROWINDOW_HPP
