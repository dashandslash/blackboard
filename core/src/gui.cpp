#include "gui.h"

#include "renderer/platform/imgui_impl_sdl_bgfx.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <imgui/backends/imgui_impl_sdl.h>
#include <SDL.h>

#include <iostream>


namespace blackboard
{
void create_window(Window<SDL_Window> &window)
{
    window.window = SDL_CreateWindow(window.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window.width, window.height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
}

bool init_ui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    //Set up input output configs
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

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

template <>
void init<SDL_Window, metal>(Window<SDL_Window> &window)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
    {
        std::cout << "Error: " << SDL_GetError() << std::endl;
    }

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
    create_window(window);
    init_ui();
    ImGui_ImplSDL2_InitForMetal(window.window);
}
}
