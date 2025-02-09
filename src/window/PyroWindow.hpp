//
// Created by srijan on 2/7/25.
//

#ifndef PYROWINDOW_HPP
#define PYROWINDOW_HPP



#include <string>
#include <SDL3/SDL_video.h>

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
        PyroWindow(int width, int
                   height, std::string &title, int options);
        ~PyroWindow();

    private:
        SDL_Window* window;

        bool should_close();
        void poll_events();

    };
} // pyro

#endif //PYROWINDOW_HPP
