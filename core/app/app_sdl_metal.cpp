#include "app.h"

#include "renderer/platform/imgui_impl_sdl_bgfx.h"
#include "renderer/renderer.h"

#include <SDL.h>
#include <bgfx/bgfx.h>
#include <imgui/backends/imgui_impl_sdl.h>

#include <iostream>

namespace blackboard {

void create_window(gui::Window<SDL_Window> &window)
{
    window.window = SDL_CreateWindow(
      window.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window.width, window.height,
      window.fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP :
                          0 | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
}

template<>
App_sdl_metal::App(const char *app_name, const uint16_t width, const uint16_t height,
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

template<>
void App_sdl_metal::run()
{
    SDL_Event event;
    while (m_running)
    {
        while (m_window.window != nullptr && SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                m_running = false;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(m_window.window))
                m_running = false;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                const auto width = event.window.data1;
                const auto height = event.window.data2;
                m_window.width = width;
                m_window.height = height;
                renderer::ImGui_Impl_sdl_bgfx_Resize(SDL_GetWindowFromID(event.window.windowID));
            }
        }
        renderer::ImGui_Impl_sdl_bgfx_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        on_update();

        ImGui::Render();
        renderer::ImGui_Impl_sdl_bgfx_Render(m_window.imgui_view_id, ImGui::GetDrawData(), 0x000000FF);

        if (const auto io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        bgfx::frame();
    }
}

template<>
std::filesystem::path App_sdl_metal::get_app_path()
{
    char *base_path = SDL_GetBasePath();
    if (base_path)
    {
        return {SDL_GetBasePath()};
    }

    return {"./"};
}

template<>
App_sdl_metal::~App()
{
    ImGui_ImplSDL2_Shutdown();
    renderer::ImGui_Impl_sdl_bgfx_Shutdown();

    ImGui::DestroyContext();
    bgfx::shutdown();

    SDL_DestroyWindow(m_window.window);
    SDL_Quit();
}
}    // namespace blackboard
