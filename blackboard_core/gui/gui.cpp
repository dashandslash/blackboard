#include "gui.h"

#include "gui/components.h"
#include "meta/meta.h"
#include "renderer/platform/imgui_impl_sdl_bgfx.h"
#include "scene/components/animation.h"
#include "scene/components/name.h"
#include "scene/components/selected.h"
#include "scene/components/transform.h"
#include "scene/components/uuid.h"
#include "state/state.h"

#include <SDL/SDL.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <entt/entity/runtime_view.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <iostream>

namespace blackboard::core::gui {
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

ImVec4 string_hex_to_rgb_float(const std::string &color)
{
    assert(color.size() == 7);
    return {std::stoul(color.substr(1, 2), nullptr, 16) / 255.0f,
            std::stoul(color.substr(3, 2), nullptr, 16) / 255.0f,
            std::stoul(color.substr(5, 2), nullptr, 16) / 255.0f, 1.0f};
}

void set_dracula_theme()
{
    ImGui::StyleColorsDark();

    static auto background = string_hex_to_rgb_float("#282a36");
    static auto selection = string_hex_to_rgb_float("#44475a");
    static auto foreground = string_hex_to_rgb_float("#f8f8f2");
    static auto comment = string_hex_to_rgb_float("#6272a4");
    static auto cyan = string_hex_to_rgb_float("#8be9fd");
    static auto green = string_hex_to_rgb_float("#50fa7b");
    static auto orange = string_hex_to_rgb_float("#ffb86c");
    static auto pink = string_hex_to_rgb_float("#ff79c6");
    static auto purple = string_hex_to_rgb_float("#bd93f9");
    static auto red = string_hex_to_rgb_float("#ff5555");
    static auto yellow = string_hex_to_rgb_float("#f1fa8c");

    const auto dark_alpha_selection = selection * ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
    const auto dark_alpha_purple = purple * ImVec4(1.0f, 1.0f, 1.0f, 0.3f);
    const auto dark_background = background * ImVec4(0.65f, 0.65f, 0.65f, 1.0f);
    const auto dark_alpha_red = red * ImVec4(1.0f, 1.0f, 1.0f, 0.10f);

    auto &colors = ImGui::GetStyle().Colors;

    const auto IconColour = ImVec4(0.718, 0.62f, 0.86f, 1.00f);
    colors[ImGuiCol_Text] = foreground;
    colors[ImGuiCol_TextSelectedBg] = comment;
    colors[ImGuiCol_TextDisabled] = string_hex_to_rgb_float("#666666");

    colors[ImGuiCol_WindowBg] = background;
    colors[ImGuiCol_ChildBg] = background;

    colors[ImGuiCol_PopupBg] = background;
    colors[ImGuiCol_Border] = dark_alpha_purple;
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = selection;
    colors[ImGuiCol_FrameBgHovered] = selection * ImVec4(1.1f, 1.1f, 1.1f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = selection * ImVec4(1.2f, 1.2f, 1.2f, 1.0f);

    colors[ImGuiCol_TitleBg] = (selection + background) * ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = (selection + background) * ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed] = (selection + background) * ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    colors[ImGuiCol_MenuBarBg] = selection;

    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab] = dark_alpha_selection;
    colors[ImGuiCol_ScrollbarGrabActive] = dark_alpha_selection * ImVec4(1.3f, 1.3f, 1.3f, 1.3f);
    colors[ImGuiCol_ScrollbarGrabHovered] = dark_alpha_selection * ImVec4(1.2f, 1.2f, 1.2f, 1.2f);

    colors[ImGuiCol_CheckMark] = comment;
    colors[ImGuiCol_SliderGrab] = comment;
    colors[ImGuiCol_SliderGrabActive] = comment * ImVec4(1.3f, 1.3f, 1.3f, 1.3f);
    colors[ImGuiCol_Button] = comment;
    colors[ImGuiCol_ButtonHovered] = comment * ImVec4(1.2f, 1.2f, 1.2f, 1.2f);
    colors[ImGuiCol_ButtonActive] = comment * ImVec4(1.3f, 1.3f, 1.3f, 1.3f);

    colors[ImGuiCol_Separator] = selection;
    colors[ImGuiCol_SeparatorHovered] = selection;
    colors[ImGuiCol_SeparatorActive] = selection;

    colors[ImGuiCol_ResizeGrip] = dark_alpha_purple;
    colors[ImGuiCol_ResizeGripHovered] = dark_alpha_purple * ImVec4(1.2f, 1.2f, 1.2f, 1.2f);
    colors[ImGuiCol_ResizeGripActive] = dark_alpha_purple * ImVec4(1.3f, 1.3f, 1.3f, 1.3f);

    colors[ImGuiCol_PlotLines] = yellow;
    colors[ImGuiCol_PlotLinesHovered] = yellow * ImVec4(1.2f, 1.2f, 1.2f, 1.2f);
    colors[ImGuiCol_PlotHistogram] = yellow;
    colors[ImGuiCol_PlotHistogramHovered] = yellow * ImVec4(1.2f, 1.2f, 1.2f, 1.2f);

    colors[ImGuiCol_DragDropTarget] = red;

    colors[ImGuiCol_NavHighlight] = red;
    colors[ImGuiCol_NavWindowingHighlight] = comment;
    colors[ImGuiCol_NavWindowingDimBg] = red;
    colors[ImGuiCol_ModalWindowDimBg] = dark_alpha_red;

    colors[ImGuiCol_Header] = dark_alpha_selection;
    colors[ImGuiCol_HeaderHovered] = dark_alpha_selection * ImVec4(1.2f, 1.2f, 1.2f, 1.2f);
    colors[ImGuiCol_HeaderActive] = dark_alpha_selection * ImVec4(1.3f, 1.3f, 1.3f, 1.3f);

    colors[ImGuiCol_Tab] = comment;
    colors[ImGuiCol_TabHovered] = comment * ImVec4(1.2f, 1.2f, 1.2f, 1.2f);
    colors[ImGuiCol_TabActive] = comment * ImVec4(1.3f, 1.3f, 1.3f, 1.3f);
    colors[ImGuiCol_TabUnfocused] = comment * ImVec4(0.5f, 0.5f, 0.5f, 0.5f);
    colors[ImGuiCol_TabUnfocusedActive] = comment * ImVec4(0.5f, 0.5f, 0.5f, 0.5f);

    colors[ImGuiCol_DockingEmptyBg] = dark_background;
    colors[ImGuiCol_DockingPreview] = dark_alpha_purple;

    colors[ImGuiCol_TableHeaderBg] = comment;
    colors[ImGuiCol_TableBorderLight] = dark_alpha_purple;
    colors[ImGuiCol_TableBorderStrong] = dark_alpha_purple;

    ImGui::GetStyle().FramePadding = {1.0f, 1.0f};
    ImGui::GetStyle().CellPadding = {2.0f, 2.0f};
    ImGui::GetStyle().TabBorderSize = 1.0f;
    ImGui::GetStyle().TabRounding = 0.0f;
}

void load_font(const std::filesystem::path &path, const float size, const bool set_as_default)
{
    ImFontConfig font_config;
    font_config.RasterizerMultiply = 1.5f;
    font_config.OversampleH = 4;
    font_config.OversampleV = 4;
    auto &io = ImGui::GetIO();
    if (path.extension() != ".ttf" && path.extension() != ".otf")
    {
        return;
    }
    io.Fonts->AddFontFromFileTTF(path.string().c_str(), size, &font_config);
    // setup default font
    if (set_as_default)
    {
        io.FontDefault = io.Fonts->Fonts.back();
    }
}

void dockspace()
{
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                    ImGuiWindowFlags_NoNavFocus;

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace Demo", nullptr, window_flags);

    ImGui::PopStyleVar(3);

    // Submit the DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    ImGui::End();
}

ImTextureID toId(bgfx::TextureHandle _handle, uint8_t _flags, uint8_t _mip)
{
    union
    {
        struct
        {
            bgfx::TextureHandle handle;
            uint8_t flags;
            uint8_t mip;
        } s;
        ImTextureID id;
    } tex;
    tex.s.handle = _handle;
    tex.s.flags = _flags;
    tex.s.mip = _mip;
    return tex.id;
}

}    // namespace blackboard::core::gui
