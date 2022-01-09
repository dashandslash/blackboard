#define IMGUI_DEFINE_MATH_OPERATORS
#include <core/app/app.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <entt/entt.hpp>

#include <filesystem>
#include <iostream>

std::filesystem::path app_path;

void set_dracula_theme()
{
    static constexpr auto max = 255.0f;
    auto &colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.1f, 0.11f, 1.0f};

    ImGui::StyleColorsDark();

    ImVec4 Titlebar = ImVec4(36.0f / max, 38.0f / max, 48.0f / max, 1.0f);
    ImVec4 TabActive = ImVec4(40.0f / max, 42.0f / max, 54.0f / max, 1.0f);
    ImVec4 TabUnactive = ImVec4(35.0f / max, 43.0f / max, 59.0f / max, 1.0f);

    const auto IconColour = ImVec4(183.0f / 255.0f, 158.0f / 255.0f, 220.0f / 255.0f, 1.00f);
    colors[ImGuiCol_Text] = ImVec4(159.0f / 255.0f, 159.0f / 255.0f, 163.0f / 255.0f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);

    colors[ImGuiCol_WindowBg] = TabActive;
    colors[ImGuiCol_ChildBg] = TabActive;

    colors[ImGuiCol_PopupBg] = ImVec4(42.0f / 255.0f, 38.0f / 255.0f, 47.0f / 255.0f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(65.0f / 255.0f, 79.0f / 255.0f, 92.0f / 255.0f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);

    colors[ImGuiCol_TitleBg] = Titlebar;
    colors[ImGuiCol_TitleBgActive] = Titlebar;
    colors[ImGuiCol_TitleBgCollapsed] = Titlebar;
    colors[ImGuiCol_MenuBarBg] = Titlebar;

    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.6f, 0.6f, 0.6f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.7f, 0.7f, 0.7f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.8f, 0.8f, 0.8f, 1.00f);

    colors[ImGuiCol_CheckMark] = ImVec4(0.608f, 0.510f, 0.812f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.608f, 0.510f, 0.812f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] =
      ImVec4(185.0f / 255.0f, 160.0f / 255.0f, 237.0f / 255.0f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(59.0f / 255.0f, 46.0f / 255.0f, 80.0f / 255.0f, 1.0f);
    colors[ImGuiCol_ButtonActive] = colors[ImGuiCol_ButtonHovered] + ImVec4(0.1f, 0.1f, 0.1f, 0.1f);

    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);

    colors[ImGuiCol_ResizeGrip] = ImVec4(0.608f, 0.510f, 0.812f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.608f, 0.510f, 0.812f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.608f, 0.510f, 0.812f, 0.95f);

    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    colors[ImGuiCol_Header] = TabActive + ImVec4(0.1f, 0.1f, 0.1f, 0.1f);
    colors[ImGuiCol_HeaderHovered] = TabActive + ImVec4(0.1f, 0.1f, 0.1f, 0.1f);
    colors[ImGuiCol_HeaderActive] = TabActive + ImVec4(0.05f, 0.05f, 0.05f, 0.1f);

    colors[ImGuiCol_HeaderActive] = TabActive + ImVec4(0.05f, 0.05f, 0.05f, 0.1f);

#ifdef IMGUI_HAS_DOCK

    colors[ImGuiCol_Tab] = TabUnactive;
    colors[ImGuiCol_TabHovered] = TabActive + ImVec4(0.1f, 0.1f, 0.1f, 0.1f);
    colors[ImGuiCol_TabActive] = TabActive;
    colors[ImGuiCol_TabUnfocused] = TabUnactive;
    colors[ImGuiCol_TabUnfocusedActive] = TabActive;
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);

#endif
    colors[ImGuiCol_Separator] = colors[ImGuiCol_TitleBg];
    colors[ImGuiCol_SeparatorActive] = colors[ImGuiCol_Separator];
    colors[ImGuiCol_SeparatorHovered] = colors[ImGuiCol_Separator];
    colors[ImGuiCol_Tab] = colors[ImGuiCol_MenuBarBg];
    colors[ImGuiCol_TabUnfocused] = colors[ImGuiCol_MenuBarBg];
    colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_WindowBg];
    colors[ImGuiCol_TabActive] = colors[ImGuiCol_WindowBg];
    colors[ImGuiCol_ChildBg] = colors[ImGuiCol_TabActive];
    colors[ImGuiCol_ScrollbarBg] = colors[ImGuiCol_TabActive];
    colors[ImGuiCol_TitleBgActive] = colors[ImGuiCol_TitleBg];
    colors[ImGuiCol_TitleBgCollapsed] = colors[ImGuiCol_TitleBg];
    colors[ImGuiCol_MenuBarBg] = colors[ImGuiCol_TitleBg];
    colors[ImGuiCol_PopupBg] = colors[ImGuiCol_WindowBg] + ImVec4(0.05f, 0.05f, 0.05f, 0.0f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 0.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
}

void load_fonts()
{
    ImFontConfig font_config;
    font_config.RasterizerMultiply = 1.15f;
    font_config.OversampleH = 4;
    font_config.OversampleV = 4;
    auto &io = ImGui::GetIO();

    const auto fonts_directory = app_path / "assets/fonts/";

    for (const auto &font_file : std::filesystem::recursive_directory_iterator{fonts_directory})
    {
        if (font_file.path().extension().string() == ".ttf")
        {
            io.Fonts->AddFontFromFileTTF(font_file.path().c_str(), 14, &font_config);
        }
        // setup default font
        if (font_file.path().stem() == "Roboto-Regular")
        {
            io.FontDefault = io.Fonts->Fonts.back();
        }
    }
}

void dockspace()
{
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of
            // other windows, which we can't undo at the moment without finer window
            // depth/z control.
            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
            ImGui::MenuItem("Padding", NULL, &opt_padding);
            ImGui::Separator();

            if (ImGui::MenuItem("Flag: NoSplit", "",
                                (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
            }
            if (ImGui::MenuItem("Flag: NoResize", "",
                                (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
            }
            if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "",
                                (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
            }
            if (ImGui::MenuItem("Flag: AutoHideTabBar", "",
                                (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
            }
            if (ImGui::MenuItem("Flag: PassthruCentralNode", "",
                                (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0,
                                opt_fullscreen))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
            }
            ImGui::Separator();

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
    ImGui::End();
}

void update()
{
    dockspace();

    ImGui::Begin("A Window");
    static int sliderInt{0};
    ImGui::SliderInt("A slider", &sliderInt, 0, 100);
    static bool showDemo{false};
    ImGui::Checkbox("Show demo window", &showDemo);
    if (showDemo)
    {
        ImGui::ShowDemoWindow();
    }
    ImGui::End();
}

int main(int argc, char *argv[])
{
    static const std::string headless_arg{"headless"};
    if (argc > 1 && std::string(argv[1]) == headless_arg)
    {
        blackboard::App_headless app(headless_arg.c_str());
        app.on_update = []() { std::cout << "Headless update" << std::endl; };
        app.run();
    }
    else
    {
        blackboard::App_sdl_metal app("Example SDL+Metal", 1280, 720, true);
        app.on_update = update;
        app_path = app.get_app_path();
        std::cout << app_path << std::endl;
        set_dracula_theme();
        load_fonts();

        app.run();
    }

    return 0;
}
