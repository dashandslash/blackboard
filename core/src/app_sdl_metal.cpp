#include "app.h"

#include "renderer/platform/imgui_impl_sdl_bgfx.h"
#include "renderer/renderer.h"

#include <SDL.h>
#include <bgfx/bgfx.h>
#include <imgui/backends/imgui_impl_sdl.h>

#include <iostream>

namespace blackboard {
template<>
App<Render_api::metal>::App(const char *app_name, const uint16_t width, const uint16_t height)
{
    m_window.title = app_name;
    init<SDL_Window, metal>(m_window);
    if (m_window.window)
    {
        renderer::init(m_window.window, m_window.width, m_window.height);
        renderer::ImGui_Impl_sdl_bgfx_Init(m_window.imgui_view_id);
    }
}

template<>
void App<Render_api::metal>::run()
{
    SDL_Event event;
    while (running)
    {
        while (m_window.window != nullptr && SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(m_window.window))
                running = false;
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
std::filesystem::path App<Render_api::metal>::get_app_path()
{
    char *base_path = SDL_GetBasePath();
    if (base_path)
    {
        return {SDL_GetBasePath()};
    }

    return {"./"};
}

template<>
App<Render_api::metal>::~App()
{
    ImGui_ImplSDL2_Shutdown();
    renderer::ImGui_Impl_sdl_bgfx_Shutdown();

    ImGui::DestroyContext();
    bgfx::shutdown();

    SDL_DestroyWindow(m_window.window);
    SDL_Quit();
}
}    // namespace blackboard
