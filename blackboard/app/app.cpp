#include "app.h"

#include "resources.h"

#include "renderer/platform/imgui_impl_sdl_bgfx.h"
#include "renderer/renderer.h"

#include <SDL.h>
#include <bgfx/bgfx.h>
#include <imgui/backends/imgui_impl_sdl.h>

#include <iostream>

namespace blackboard {

App::App(const char *app_name, const renderer::Api renderer_api, const uint16_t width,
         const uint16_t height, const bool fullscreen)
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
    m_window.init_platform_window();

    gui::init();

    if (m_window.window)
    {
        renderer::init(m_window.window, renderer_api, m_window.width, m_window.height);
        renderer::ImGui_Impl_sdl_bgfx_Init(m_window.imgui_view_id);
    }

    ImGui_ImplSDL2_InitForMetal(m_window.window);
}

void App::run()
{
    on_init();
    ImGui::LoadIniSettingsFromDisk((resources::path() / "imgui.ini").string().c_str());
    int drawable_width{0};
    int drawable_height{0};
    SDL_Metal_GetDrawableSize(m_window.window, &drawable_width, &drawable_height);
    SDL_GetWindowPosition(m_window.window, &m_window.window_x, &m_window.window_y);
    on_resize(drawable_width, drawable_height);

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
                auto window = SDL_GetWindowFromID(event.window.windowID);
                renderer::ImGui_Impl_sdl_bgfx_Resize(window);
                int drawable_width{0};
                int drawable_height{0};
                SDL_Metal_GetDrawableSize(window, &drawable_width, &drawable_height);
                on_resize(drawable_width, drawable_height);
            }
#if 0
            if (event.type == SDL_MOUSEMOTION) {
                m_window.prev_mouse_x = m_window.mouse_x;
                m_window.prev_mouse_y = m_window.mouse_y;
                SDL_GetGlobalMouseState( &m_window.mouse_x, &m_window.mouse_y );

                if (m_window.is_dragging) {
                    m_window.window_x += m_window.mouse_x - m_window.prev_mouse_x;
                    m_window.window_y += m_window.mouse_y - m_window.prev_mouse_y;
                    
                    SDL_SetWindowPosition(m_window.window, m_window.window_x, m_window.window_y);
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                m_window.prev_mouse_x = m_window.mouse_x;
                m_window.prev_mouse_y = m_window.mouse_y;
                SDL_GetGlobalMouseState( &m_window.mouse_x, &m_window.mouse_y );
                m_window.is_dragging = true;
            }
            if (event.type == SDL_MOUSEBUTTONUP)
            {
                m_window.prev_mouse_x = m_window.mouse_x;
                m_window.prev_mouse_y = m_window.mouse_y;
                SDL_GetGlobalMouseState( &m_window.mouse_x, &m_window.mouse_y );
                m_window.is_dragging = false;
            }
#endif
        }
        renderer::ImGui_Impl_sdl_bgfx_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        on_update();
        m_prev_time = std::chrono::steady_clock::now();

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

App::~App()
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
