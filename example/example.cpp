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
std::filesystem::path imgui_ini_path;

bgfx::VertexBufferHandle vbh;
bgfx::IndexBufferHandle ibh;

blackboard::renderer::CameraPersp cam;

bgfx::FrameBufferHandle frameBufferHandle = BGFX_INVALID_HANDLE;

using namespace blackboard::renderer::layouts;

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
        ImGui::Image(blackboard::gui::toId(bgfx::getTexture(frameBufferHandle), UINT8_C(0x01), 0),
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
        imgui_ini_path = app_path / "imgui.ini";

        blackboard::gui::set_dracula_theme();
        load_fonts();

        ImGui::LoadIniSettingsFromDisk(imgui_ini_path.string().c_str());
        app.run();
        ImGui::SaveIniSettingsToDisk(imgui_ini_path.string().c_str());
    }

    return 0;
}
