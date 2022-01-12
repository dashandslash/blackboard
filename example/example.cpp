#define IMGUI_DEFINE_MATH_OPERATORS
#include <core/app/app.h>
#include <core/renderer/camera.h>
#include <core/renderer/layouts.h>
#include <core/renderer/renderer.h>
#include <core/renderer/utils.h>

#include <entt/entt.hpp>
#include <glm/ext.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <filesystem>
#include <iostream>
#include <sstream>

std::filesystem::path app_path;

bgfx::VertexBufferHandle vbh;
bgfx::IndexBufferHandle ibh;

blackboard::renderer::CameraPersp cam;

bgfx::FrameBufferHandle frameBufferHandle = BGFX_INVALID_HANDLE;

using namespace blackboard::renderer::layouts;

// input format #aa11ff
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

    const auto background = string_hex_to_rgb_float("#282a36");
    const auto selection = string_hex_to_rgb_float("#44475a");
    const auto foreground = string_hex_to_rgb_float("#f8f8f2");
    const auto comment = string_hex_to_rgb_float("#6272a4");
    const auto cyan = string_hex_to_rgb_float("#8be9fd");
    const auto green = string_hex_to_rgb_float("#50fa7b");
    const auto orange = string_hex_to_rgb_float("#ffb86c");
    const auto pink = string_hex_to_rgb_float("#ff79c6");
    const auto purple = string_hex_to_rgb_float("#bd93f9");
    const auto red = string_hex_to_rgb_float("#ff5555");
    const auto yellow = string_hex_to_rgb_float("#f1fa8c");

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
    ;
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
        if (font_file.path().extension() == ".ttf" || font_file.path().extension() == ".otf")
        {
            io.Fonts->AddFontFromFileTTF(font_file.path().c_str(), 14, &font_config);
        }
        // setup default font
        if (font_file.path().stem() == "SourceSansPro-Regular")
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

inline ImTextureID toId(bgfx::TextureHandle _handle, uint8_t _flags, uint8_t _mip)
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

void update()
{
    dockspace();

    ImGui::Begin("A Window");
    static int sliderInt{0};
    ImGui::SliderInt("A slider", &sliderInt, 0, 100);
    static bool showDemo{false};
    ImGui::Checkbox("Show demo window", &showDemo);

    ImVec2 size(ImGui::GetContentRegionAvail().x,
                ImGui::GetContentRegionAvail().x / (1280.0f / 720.0f));
    if (bgfx::isValid(frameBufferHandle))
    {
        ImGui::Image(toId(bgfx::getTexture(frameBufferHandle), UINT8_C(0x01), 0),
                     ImVec2(ImGui::GetContentRegionAvail().x,
                            ImGui::GetContentRegionAvail().x / (1280.0f / 720.0f)),
                     ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                     ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    }
    //    ImGui::Image(bgfx::getTexture(frameBufferHandle), ImVec2(128.0f, 128.0f) );
    if (showDemo)
    {
        ImGui::ShowDemoWindow();
    }

    cam.setEyePoint({0.0, 0.0, -8.0f});
    cam.setPerspective(40.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
    cam.lookAt({0.0f, 0.0f, 0.0f});

    const auto &view = cam.getViewMatrix();
    const auto &proj = cam.getProjectionMatrix();
    bgfx::setViewTransform(5, glm::value_ptr(view), glm::value_ptr(proj));
    bgfx::setViewClear(5, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);

    float mtx[16];
    static float time{0.0f};
    time += 0.0073f;
    bx::mtxSRT(mtx, 1.0f, 1.0f, 1.0f, time * 2.0f, time * 1.3f, time, 0.0f, 0.0f, 0.0f);

    bgfx::setVertexBuffer(0, vbh);
    bgfx::setIndexBuffer(ibh);
    bgfx::setTransform(mtx);
    bgfx::setState(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
                   BGFX_STATE_DEPTH_TEST_LESS | /*BGFX_STATE_CULL_CCW |*/ BGFX_STATE_MSAA);
    auto prog =
      blackboard::renderer::material_manager().material<blackboard::renderer::material::UniformColor>();

    static blackboard::renderer::material::Uniform uniform = {
      .u_color = {1.0, 1.0, 1.0, 1.0}, .u_edge_thickness = 3.5f, .u_edge_color{0.0f, 0.0f, 0.0f, 1.0f}};
    uniform.u_camera_position = {cam.getEyePoint().x, cam.getEyePoint().y, cam.getEyePoint().z};
    uniform.u_time = blackboard::App_sdl_metal::elapsed_time();

    blackboard::renderer::material_manager().set_uniform(&uniform);

    bgfx::submit(5, prog->program_handle());

    ImGui::ColorEdit4("u_color", uniform.u_color.data());
    ImGui::ColorEdit4("u_edge_color", uniform.u_edge_color.data());
    ImGui::SliderFloat("u_edge_thickness", &uniform.u_edge_thickness, 0.0f, 20.0f);
    ImGui::End();
}

void resize(const uint16_t w, const uint16_t h)
{
    static const uint64_t tsFlags = 0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT |
                                    BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT |
                                    BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
    std::array<bgfx::TextureHandle, 2> attachments;
    attachments[0] = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::RGBA8, tsFlags, NULL);

    attachments[1] = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::D24S8, tsFlags, NULL);
    frameBufferHandle = bgfx::createFrameBuffer(attachments.size(), attachments.data());
    bgfx::setViewRect(5, 0, 0, w, h);
    bgfx::setViewFrameBuffer(5, frameBufferHandle);
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
        blackboard::App_sdl_metal app("Example SDL+Metal");
        app.on_update = update;
        app.on_resize = resize;

        using namespace blackboard::renderer::layouts;

        vbh = bgfx::createVertexBuffer(
          bgfx::copy(cube_position_normal_barycenter.data(),
                     sizeof(decltype(cube_position_normal_barycenter)::value_type) *
                       cube_position_normal_barycenter.size()),
          Position_normal_barycenter::layout());
        ibh = bgfx::createIndexBuffer(bgfx::copy(
          cube_indices.data(), sizeof(decltype(cube_indices)::value_type) * cube_indices.size()));

        app_path = app.get_app_path();
        std::cout << app_path << std::endl;
        set_dracula_theme();
        load_fonts();

        app.run();
    }

    return 0;
}
