#include "window.h"

#include <SDL/SDL.h>

namespace blackboard {

template<>
void Window<SDL_Window>::init_platform_window()
{
    window = SDL_CreateWindow(
      title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
      (fullscreen ? (SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS) : SDL_WINDOW_SHOWN) |
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    //    SDL_SetWindowBordered(window.window, SDL_FALSE);
}

}    // namespace blackboard
