#include "window.h"

#include <SDL/SDL.h>

namespace blackboard::core {

void Window::init_platform_window()
{
    window = SDL_CreateWindow(
      title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
      (fullscreen ? (SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS) : SDL_WINDOW_SHOWN) |
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    //    SDL_SetWindowBordered(window.window, SDL_FALSE);
}

std::pair<uint16_t, uint16_t> Window::get_size_in_pixels() const
{
    int w{0u}, h{0u};
    SDL_GL_GetDrawableSize(window, &w, &h);
    return {w, h};
}

std::pair<uint16_t, uint16_t> Window::get_position() const
{
    int x{0u}, y{0u};
    SDL_GetWindowPosition(window, &x, &y);
    return {x, y};
}

}    // namespace blackboard::core
