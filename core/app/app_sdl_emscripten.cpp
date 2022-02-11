#ifdef __EMSCRIPTEN__
#include "app.h"

#include "resources.h"

#include "renderer/platform/imgui_impl_sdl_bgfx.h"
#include "renderer/renderer.h"

#include <SDL.h>
#include <SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <imgui/backends/imgui_impl_sdl.h>

#include <emscripten.h>

#include <iostream>

namespace blackboard {

void create_window(gui::Window<SDL_Window> &window)
{
    window.window =
      SDL_CreateWindow(window.title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       window.width, window.height, SDL_WINDOW_SHOWN);
    //    SDL_SetWindowBordered(window.window, SDL_FALSE);
}

template<>
App_sdl_emscripten::App(const char *app_name, const uint16_t width, const uint16_t height,
                        const bool fullscreen)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
    {
        std::cout << "Error: " << SDL_GetError() << std::endl;
    }

    m_window.title = app_name;
    m_window.width = width;
    m_window.height = height;
    m_window.fullscreen = fullscreen;

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
    create_window(m_window);

    gui::init();

    ImGui_ImplSDL2_InitForMetal(m_window.window);

    if (m_window.window)
    {
        renderer::init(m_window.window, m_window.width, m_window.height);
        renderer::ImGui_Impl_sdl_bgfx_Init(m_window.imgui_view_id);
    }
}

void loop(void *data)
{
    std::cout << "> loop" << std::endl;
    auto app = (App_sdl_emscripten *)data;

    for (SDL_Event currentEvent; SDL_PollEvent(&currentEvent) != 0;)
    {
        std::cout << "> event" << std::endl;
        ImGui_ImplSDL2_ProcessEvent(&currentEvent);
        // if (currentEvent.type == SDL_QUIT) {
        //     context->quit = true;
        //     break;
        // }
    }

    renderer::ImGui_Impl_sdl_bgfx_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    app->on_update();
    app->m_prev_time = std::chrono::steady_clock::now();

    ImGui::Render();
    renderer::ImGui_Implbgfx_RenderDrawLists(app->m_window.imgui_view_id, ImGui::GetDrawData());

    // if (const auto io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    // {
    //     ImGui::UpdatePlatformWindows();
    //     ImGui::RenderPlatformWindowsDefault();
    // }

    bgfx::frame();
    // SDL_GL_SwapWindow(app->m_window.window);
}

template<>
void App_sdl_emscripten::run()
{
    on_init();
    ImGui::LoadIniSettingsFromDisk((resources::path() / "imgui.ini").string().c_str());
    int drawable_width{0};
    int drawable_height{0};
    SDL_GL_GetDrawableSize(m_window.window, &drawable_width, &drawable_height);
    SDL_GetWindowPosition(m_window.window, &m_window.window_x, &m_window.window_y);
    on_resize(drawable_width, drawable_height);

    std::cout << "> RUN" << std::endl;
    emscripten_set_main_loop_arg(loop, this, -1, 1);
}

template<>
std::filesystem::path App_sdl_emscripten::resources_path()
{
    return resources::path();
}

template<>
App_sdl_emscripten::~App()
{
    ImGui::SaveIniSettingsToDisk((resources::path() / "imgui.ini").string().c_str());

    renderer::material_manager().shutdown();

    ImGui_ImplSDL2_Shutdown();
    renderer::ImGui_Impl_sdl_bgfx_Shutdown();

    ImGui::DestroyContext();
    bgfx::shutdown();

    SDL_DestroyWindow(m_window.window);
    SDL_Quit();
}
}    // namespace blackboard
#endif
