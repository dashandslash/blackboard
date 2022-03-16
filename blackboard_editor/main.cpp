#define SDL_MAIN_HANDLED

#include "editor/editor.h"

#include <blackboard_core/app/app.h>
#include <blackboard_core/factories/factories.h>
#include <blackboard_core/gui/components.h>
#include <blackboard_core/meta/meta.h>
#include <blackboard_core/renderer/camera.h>
#include <blackboard_core/renderer/layouts.h>
#include <blackboard_core/renderer/renderer.h>
#include <blackboard_core/renderer/utils.h>
#include <blackboard_core/resources/mesh.h>
#include <blackboard_core/resources/resources.h>
#include <blackboard_core/scene/components/resource.h>
#include <blackboard_core/scene/components/transform.h>
#include <blackboard_core/state/state.h>

#include <entt/entt.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <imgui/imgui.h>

#include <filesystem>
#include <iostream>
#include <sstream>

using namespace blackboard;
using namespace blackboard::core::renderer::layouts;

blackboard::core::renderer::CameraPersp cam;
bgfx::FrameBufferHandle frameBufferHandle = BGFX_INVALID_HANDLE;
core::renderer::material::Uniform uniform = {.u_color = {1.0, 1.0, 1.0, 1.0},
                                             .u_edge_color{0.0f, 0.0f, 0.0f, 1.0f},
                                             .u_edge_thickness = 3.5f};

entt::id_type model_key{entt::null};

static const std::string state_name{"default_state"};

void init()
{
    using namespace core::renderer::layouts;

    const auto imgui_ini_path = (core::resources::path() / "imgui.ini").string();

    core::gui::set_dracula_theme();
    const auto fonts_path = core::resources::path() / "assets/fonts";
    core::gui::load_font(fonts_path / "Source_Sans_Pro/SourceSansPro-Regular.ttf", 15.0f, true);
    core::gui::load_font(fonts_path / "Roboto/Roboto-Regular.ttf", 14.0f);
    core::gui::load_font(fonts_path / "Dejavu-sans/DejaVuSans.ttf", 14.0f);
    core::gui::load_font(fonts_path / "Dejavu-sans/DejaVuSansCondensed.ttf", 14.0f);
    core::gui::load_font(fonts_path / "IBM_Plex_Sans/IBMPlexSans-Regular.ttf", 14.0f);

    ImGui::LoadIniSettingsFromDisk(imgui_ini_path.c_str());

    auto &state = core::create_state(state_name);

    auto e = state.create_entity();
    auto &tr_start = state.emplace_component<core::components::Transform>(e);

    model_key =
      core::resources::load_model(core::resources::path() / "assets/models/Sponza/glTF/Sponza.gltf");

    if (core::resources::is_valid_model_key(model_key))
    {
        for (auto &&mesh : core::resources::get_model_ref(model_key).meshes)
        {
            mesh.vbh = bgfx::createVertexBuffer(
              bgfx::makeRef(mesh.vertices.data(),
                            sizeof(core::resources::Vertex) * mesh.vertices.size()),
              Position_normal_tangent_bitangent_color_uv::layout());
            mesh.ibh = bgfx::createIndexBuffer(
              bgfx::makeRef(mesh.indices.data(), sizeof(uint32_t) * mesh.indices.size()),
              BGFX_BUFFER_INDEX32);
        }

        state.emplace_component<core::components::Resource_key<core::resources::Model>>(e, model_key);
    }

    editor::init();

    cam.setEyePoint({0.0, 0.0, -1200.0f});
    cam.setPerspective(40.0f, 1280.0f / 720.0f, 0.1f, 100000.0f);
    cam.lookAt({0.0f, 0.0f, 0.0f});
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

    cam.setEyePoint({0.0, 0.0, -50.0f});
    cam.setPerspective(40.0f, static_cast<float>(w) / static_cast<float>(h), 0.1f, 1000.0f);
    cam.lookAt({0.0f, 0.0f, 0.0f});
}

void render_ui()
{
    editor::dockspace();
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("u_color", uniform.u_color.data());
    ImGui::ColorEdit4("u_edge_color", uniform.u_edge_color.data());
    ImGui::SliderFloat("u_edge_thickness", &uniform.u_edge_thickness, 0.0f, 20.0f);
    ImGui::End();

    auto &state = core::get_state(state_name);

    editor::entities_window(state);

    ImGui::Begin("Viewport");

    static ImVec2 curr_size{};

    if (curr_size.x != ImGui::GetContentRegionAvail().x ||
        curr_size.y != ImGui::GetContentRegionAvail().y)
    {
        curr_size = ImGui::GetContentRegionAvail();

        resize(ImGui::GetContentRegionAvail().x * ImGui::GetWindowDpiScale(),
               ImGui::GetContentRegionAvail().y * ImGui::GetWindowDpiScale());
    }

    ImVec2 size(ImGui::GetContentRegionAvail().x,
                ImGui::GetContentRegionAvail().x / (1280.0f / 720.0f));
    if (bgfx::isValid(frameBufferHandle))
    {
        ImGui::Image(core::gui::toId(bgfx::getTexture(frameBufferHandle), UINT8_C(0x01), 0), curr_size,
                     ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                     ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    }
    ImGui::End();
}

void app_update()
{
    const auto &view = cam.getViewMatrix();
    const auto &proj = cam.getProjectionMatrix();
    bgfx::setViewTransform(5, glm::value_ptr(view), glm::value_ptr(proj));
    bgfx::setViewClear(5, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);

    uniform.u_camera_position = {cam.getEyePoint().x, cam.getEyePoint().y, cam.getEyePoint().z};
    uniform.u_time = core::App::elapsed_time();

    auto &state = core::get_state(state_name);

    state.view<core::components::Transform, core::components::model_resource_key>().each(
      [&](const auto, const auto &transform, const auto &key) {
          bgfx::setState(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
                         BGFX_STATE_DEPTH_TEST_LESS | /*BGFX_STATE_CULL_CCW |*/ BGFX_STATE_MSAA);
          auto prog =
            core::renderer::material_manager().material<core::renderer::material::UniformColor>();

          core::renderer::material_manager().set_uniform(&uniform);

          for (auto &&mesh : core::resources::get_model_ref(key).meshes)
          {
              bgfx::setVertexBuffer(0, mesh.vbh);
              bgfx::setIndexBuffer(mesh.ibh);
              bgfx::setTransform(glm::value_ptr(transform.get_transform()));
              bgfx::submit(5, prog->program_handle());
          }
      });

    render_ui();
}

int main(int argc, char *argv[])
{
    static const std::string headless_arg{"headless"};
    if (argc > 1 && std::string(argv[1]) == headless_arg)
    {
        core::App app(headless_arg.c_str(), core::renderer::Api::none);
        app.run();
    }
    else
    {
        core::App app("Example SDL", core::renderer::Api::count);    // autodetect renderer api
        app.on_update = app_update;
        //        app.on_resize = resize;
        app.on_init = init;
        app.run();
    }

    return 0;
}
