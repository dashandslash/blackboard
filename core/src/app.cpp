#include "app.h"

#include "renderer/platform/imgui_impl_sdl_bgfx.h"
#include "renderer/renderer.h"

#include <bgfx/bgfx.h>
#include <imgui/backends/imgui_impl_sdl.h>
#include <SDL.h>

static const uint16_t imgui_view_id{255};

namespace blackboard
{
App::App(const char* app_name)
{
    m_window = gui::init(app_name);
    if(m_window.main_window)
    {
        renderer::init(m_window.main_window.get(), m_window.width, m_window.height);
    }
}

void App::run()
{
    SDL_Event event;
    bool done = false;
    while (!done)
    {
        while (SDL_PollEvent(&event))
        {
            if(m_window.main_window)
            {
                ImGui_ImplSDL2_ProcessEvent(&event);
                if (event.type == SDL_QUIT)
                    done = true;
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(m_window.main_window.get()))
                    done = true;
                if(event.type == SDL_WINDOWEVENT) {
                        if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                            const auto width = event.window.data1;
                            const auto height = event.window.data2;
                            m_window.width = width;
                            m_window.height = height;
                            SDL_SetWindowSize(m_window.main_window.get(), width, height);
                            bgfx::reset(width, height, BGFX_RESET_VSYNC);
                            bgfx::setViewClear(imgui_view_id, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000FF, 1.0f, 0);
                            bgfx::setViewRect(imgui_view_id, 0, 0, width, height);
                        }
                    }
            }
        }
        renderer::ImGui_Impl_sdl_bgfx_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        // call to virtual update function
        update();
        
        ImGui::Render();
        renderer::ImGui_Impl_sdl_bgfx_Render(imgui_view_id, ImGui::GetDrawData(), 0);
        
        if (const auto io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
        bgfx::touch(0);
        bgfx::frame();
    }
}
    
App::~App()
{
    ImGui_ImplSDL2_Shutdown();
    renderer::ImGui_Impl_sdl_bgfx_Shutdown();

    ImGui::DestroyContext();
    bgfx::shutdown();

    SDL_DestroyWindow(m_window.main_window.get());
    SDL_Quit();
}
}
