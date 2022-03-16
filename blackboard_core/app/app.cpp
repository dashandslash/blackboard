#include "app.h"

#include "resources.h"

#include "renderer/platform/imgui_impl_sdl_bgfx.h"
#include "renderer/renderer.h"

#include <SDL/SDL.h>
#include <bgfx/bgfx.h>
#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/imgui_internal.h>
#include <imguizmo/imguizmo.h>

#include <iostream>

namespace blackboard::core {

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

    renderer::init(m_window, renderer_api, m_window.width, m_window.height);
    renderer::ImGui_Impl_sdl_bgfx_Init(m_window.imgui_view_id);

    ImGui_ImplSDL2_InitForMetal(m_window.window);
}

void App::run()
{
    on_init();
    auto layout_ui = resources::path() / "imgui.ini";
    if (!std::filesystem::exists(layout_ui))
    {
        layout_ui = resources::path() / "assets/layouts/default_imgui.ini";
    }
    ImGui::LoadIniSettingsFromDisk(layout_ui.string().c_str());
    const auto [drawable_width, drawable_height] = m_window.get_size_in_pixels();
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
                renderer::ImGui_Impl_sdl_bgfx_Resize(m_window.window);
                const auto [drawable_width, drawable_height] = m_window.get_size_in_pixels();
                on_resize(drawable_width, drawable_height);
            }
        }

        renderer::ImGui_Impl_sdl_bgfx_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();

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
}    // namespace blackboard::core
