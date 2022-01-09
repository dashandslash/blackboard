#include "gui.h"

#include "renderer/platform/imgui_impl_sdl_bgfx.h"

#include <SDL.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <imgui/backends/imgui_impl_sdl.h>

#include <iostream>

namespace blackboard::gui {
void init()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Set up input output configs
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport /
      // Platform Windows
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

    ImGui::StyleColorsDark();

    // Setup styles
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        style.FrameBorderSize = 0.f;
        style.FramePadding = ImVec2(0.f, 0.f);
    }
}
}    // namespace blackboard::gui
