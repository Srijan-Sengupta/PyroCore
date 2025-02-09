//
// Created by srijan on 2/7/25.
//

#include "PyroWindow.hpp"

#include <string>
#include <SDL3/SDL.h>

namespace pyro {
    PyroWindow::PyroWindow(int width, int height, std::string &title, int options) {
        int sdl_options = 0;
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

        sdl_options |= (options & WINDOW_FULLSCREEN)? SDL_WINDOW_FULLSCREEN : 0;
        sdl_options |= (options & WINDOW_NOT_RESIZABLE)? 0: SDL_WINDOW_RESIZABLE;
        sdl_options |= (options & WINDOW_BORDERLESS)? SDL_WINDOW_BORDERLESS : 0;
        sdl_options |= (options & WINDOW_HIDDEN)? SDL_WINDOW_HIDDEN : 0;
        sdl_options |= (options & WINDOW_MAXIMIZE)? SDL_WINDOW_MAXIMIZED : SDL_WINDOW_MINIMIZED;
        sdl_options |= (options & WINDOW_ALWAYS_ON_TOP)? SDL_WINDOW_ALWAYS_ON_TOP : 0;
        sdl_options |= (options & WINDOW_UNFOCUSED)? SDL_WINDOW_NOT_FOCUSABLE: SDL_WINDOW_INPUT_FOCUS;

        window = SDL_CreateWindow(title.c_str(), width, height, sdl_options);
    }

    PyroWindow::~PyroWindow() {
    }

    bool PyroWindow::should_close() {
        return true;
    }


} // pyro
