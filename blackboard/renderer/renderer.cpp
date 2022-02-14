#include "renderer.h"

#include "platform/imgui_impl_sdl_bgfx.h"

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <imgui/backends/imgui_impl_sdl.h>

#include <iostream>

namespace blackboard {
namespace renderer {

bool init(SDL_Window *window, const Api renderer_api, const uint16_t width, const uint16_t height)
{
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window, &wmi))
    {
        std::cout << "SDL_SysWMinfo could not be retrieved. SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    bgfx::PlatformData pd{};
    bgfx::Init bgfx_init;
    bgfx::renderFrame();    // single threaded mode
    switch (renderer_api)
    {
        case Api::metal:
            bgfx_init.type = bgfx::RendererType::Metal;    // auto choose renderer
            break;
        case Api::d3d11:
            bgfx_init.type = bgfx::RendererType::Direct3D11;    // auto choose renderer
            break;
        case Api::webgpu:
            bgfx_init.type = bgfx::RendererType::WebGPU;    // auto choose renderer
            break;
        default:
            bgfx_init.type = bgfx::RendererType::Count;    // auto choose renderer
            break;
    }
    int width_{0};
    int height_{0};
    SDL_GL_GetDrawableSize(window, &width_, &height_);
    bgfx_init.resolution.width = width_;
    bgfx_init.resolution.height = height_;
    pd.nwh = renderer::native_window_handle(window);
    bgfx_init.resolution.numBackBuffers = 1;

    bgfx_init.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_HIDPI | BGFX_RESET_MSAA_X4;
    bgfx_init.platformData = pd;
    bgfx::init(bgfx_init);

    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, width_, height_);

    return true;
}
}    // namespace renderer
}    // namespace blackboard
