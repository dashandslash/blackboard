#include "renderer.h"

#include "platform/imgui_impl_sdl_bgfx.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <imgui/backends/imgui_impl_sdl.h>
#include <SDL.h>
#include <SDL_syswm.h>

#include <iostream>


namespace blackboard
{
namespace renderer
{

bool init(SDL_Window* window, const uint16_t width, const uint16_t height)
{
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window, &wmi)) {
        std::cout << "SDL_SysWMinfo could not be retrieved. SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    bgfx::renderFrame(); // single threaded mode
    bgfx::PlatformData pd{};
    pd.nwh = renderer::native_window_handle(window);
    pd.context = NULL;
    pd.backBuffer = NULL;
    pd.backBufferDS = NULL;
    bgfx::Init bgfx_init;
    bgfx_init.type = bgfx::RendererType::Count; // auto choose renderer
    int width_{0};
    int height_{0};
    SDL_GL_GetDrawableSize(window, &width_, &height_);
    bgfx_init.resolution.width = width_;
    bgfx_init.resolution.height = height_;
    bgfx_init.resolution.numBackBuffers = 1;
    bgfx_init.platformData = pd;
    bgfx_init.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_HIDPI;
    bgfx::init(bgfx_init);
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, width, height);
    
    return true;
}
}
}
