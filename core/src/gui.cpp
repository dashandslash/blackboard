#include "gui.h"

#include "renderer/platform/imgui_impl_sdl_bgfx.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <imgui/backends/imgui_impl_sdl.h>
#include <SDL.h>
#include <SDL_syswm.h>


namespace blackboard
{
namespace gui
{
bool init_main_window(Window_data &window, const std::string &title)
{
    window.main_window = std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>>(SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window.width, window.height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE), SDL_DestroyWindow);
    if(!window.main_window)
    {
        printf("Error creating window: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

bool init_ui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    //Set up input output configs
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

    //DARK MODE POG
    ImGui::StyleColorsDark();

    //Setup styles
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        style.FrameBorderSize = 0.f;
        style.FramePadding = ImVec2(0.f, 0.f);
    }
    return true;
}

Window_data init(const std::string &window_title) noexcept
{
    Window_data window;
    if (SDL_Init(0 | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
    }
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");

    init_main_window(window, window_title);
    
    init_ui();
    
    ImGui_ImplSDL2_InitForMetal(window.main_window.get());
    renderer::ImGui_Impl_sdl_bgfx_Init(255);
    
    return window;
}
}
}
