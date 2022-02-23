#define SDL_MAIN_HANDLED

#include <blackboard/app/app.h>
#include <blackboard/meta/meta.h>
#include <blackboard/renderer/camera.h>
#include <blackboard/renderer/layouts.h>
#include <blackboard/renderer/renderer.h>
#include <blackboard/renderer/utils.h>
#include <blackboard/scene/components/animation.h>
#include <blackboard/scene/components/transform.h>
#include <blackboard/state/state.h>

#include <entt/entt.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <imgui/imgui.h>

#include <filesystem>
#include <iostream>
#include <sstream>

using namespace blackboard;
using namespace blackboard::renderer::layouts;

bgfx::VertexBufferHandle vbh;
bgfx::IndexBufferHandle ibh;
renderer::CameraPersp cam;
bgfx::FrameBufferHandle frameBufferHandle = BGFX_INVALID_HANDLE;
renderer::material::Uniform uniform = {.u_color = {1.0, 1.0, 1.0, 1.0},
                                       .u_edge_color{0.0f, 0.0f, 0.0f, 1.0f},
                                       .u_edge_thickness = 3.5f};
State state;

void init()
{
    using namespace renderer::layouts;

    vbh = bgfx::createVertexBuffer(
      bgfx::makeRef(cube_position_normal_barycenter.data(),
                    sizeof(decltype(cube_position_normal_barycenter)::value_type) *
                      cube_position_normal_barycenter.size()),
      Position_normal_barycenter::layout());
    ibh = bgfx::createIndexBuffer(bgfx::makeRef(
      cube_indices.data(), sizeof(decltype(cube_indices)::value_type) * cube_indices.size()));

    const auto imgui_ini_path = (resources::path() / "imgui.ini").string();

    gui::set_dracula_theme();
    const auto fonts_path = resources::path() / "assets/fonts";
    gui::load_font(fonts_path / "Source_Sans_Pro/SourceSansPro-Regular.ttf", 15.0f, true);
    gui::load_font(fonts_path / "Roboto/Roboto-Regular.ttf", 14.0f);
    gui::load_font(fonts_path / "Dejavu-sans/DejaVuSans.ttf", 14.0f);
    gui::load_font(fonts_path / "Dejavu-sans/DejaVuSansCondensed.ttf", 14.0f);
    gui::load_font(fonts_path / "IBM_Plex_Sans/IBMPlexSans-Regular.ttf", 14.0f);

    ImGui::LoadIniSettingsFromDisk(imgui_ini_path.c_str());

    auto e = state.create_entity();
    auto &tr_start = state.emplace_component<components::Transform>(e);
    tr_start.translation = {-5.0f, 0.0f, 0.0f};
    tr_start.rotation = glm::quat({-350.0f, -0.0f, -359.0f});
    auto tr_end = components::Transform();
    tr_end.rotation = glm::quat({350.0f, 0.0f, 359.0f});
    tr_end.translation = glm::vec3{5.0f, 0.0f, 0.0f};

    auto &anim_comp = state.emplace_component<components::Animation<components::Transform>>(
      e, 3000.0f, tr_start, tr_end, blackboard::Easing::OutExpo);
    anim_comp.ping_pong = true;
    anim_comp.loop = true;
}

void render_ui()
{
    gui::dockspace();
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("u_color", uniform.u_color.data());
    ImGui::ColorEdit4("u_edge_color", uniform.u_edge_color.data());
    ImGui::SliderFloat("u_edge_thickness", &uniform.u_edge_thickness, 0.0f, 20.0f);
    ImGui::End();

    ImGui::Begin("Viewport");
    ImVec2 size(ImGui::GetContentRegionAvail().x,
                ImGui::GetContentRegionAvail().x / (1280.0f / 720.0f));
    if (bgfx::isValid(frameBufferHandle))
    {
        ImGui::Image(gui::toId(bgfx::getTexture(frameBufferHandle), UINT8_C(0x01), 0),
                     ImVec2(ImGui::GetContentRegionAvail().x,
                            ImGui::GetContentRegionAvail().x / (1280.0f / 720.0f)),
                     ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                     ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    }
    ImGui::End();
}

void update()
{
    cam.setEyePoint({0.0, 0.0, -12.0f});
    cam.setPerspective(40.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
    cam.lookAt({0.0f, 0.0f, 0.0f});

    const auto &view = cam.getViewMatrix();
    const auto &proj = cam.getProjectionMatrix();
    bgfx::setViewTransform(5, glm::value_ptr(view), glm::value_ptr(proj));
    bgfx::setViewClear(5, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);

    uniform.u_camera_position = {cam.getEyePoint().x, cam.getEyePoint().y, cam.getEyePoint().z};
    uniform.u_time = App::elapsed_time();

    state.view<components::Transform, components::Animation<components::Transform>>().each(
      [](const auto, components::Transform &transform,
         components::Animation<components::Transform> &anim) {
          anim.tick(App::delta_time());

          transform = anim.current_value();
          if (anim.completed)
          {
              anim.set_easing(static_cast<Easing>((anim.get_easing() + 1) % Easing::Count));
              anim.reset();
          }
      });

    state.view<components::Transform>().each([](const auto, components::Transform transform) {
        auto mtx = glm::value_ptr(transform.get_transform());

        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);
        bgfx::setTransform(mtx);
        bgfx::setState(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
                       BGFX_STATE_DEPTH_TEST_LESS | /*BGFX_STATE_CULL_CCW |*/ BGFX_STATE_MSAA);
        auto prog = renderer::material_manager().material<renderer::material::UniformColor>();

        renderer::material_manager().set_uniform(&uniform);

        bgfx::submit(5, prog->program_handle());
    });

    render_ui();
}

void resize(const uint16_t w, const uint16_t h)
{
    if (bgfx::isValid(frameBufferHandle))
    {
        bgfx::destroy(frameBufferHandle);
    }
    static const uint64_t tsFlags = 0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT |
                                    BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT |
                                    BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
    std::array<bgfx::TextureHandle, 2> attachments;
    attachments[0] = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::RGBA8, tsFlags, NULL);
    attachments[1] = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::D24S8, tsFlags, NULL);
    frameBufferHandle = bgfx::createFrameBuffer(attachments.size(), attachments.data(), true);
    bgfx::setViewRect(5, 0, 0, w, h);
    bgfx::setViewFrameBuffer(5, frameBufferHandle);
}

int main(int argc, char *argv[])
{
    static const std::string headless_arg{"headless"};
    if (argc > 1 && std::string(argv[1]) == headless_arg)
    {
        App app(headless_arg.c_str(), blackboard::renderer::Api::none);
        app.run();
    }
    else
    {
        App app("Example SDL", blackboard::renderer::Api::count);    // autodetect renderer api
        app.on_update = update;
        app.on_resize = resize;
        app.on_init = init;
        app.run();
    }

    return 0;
}
