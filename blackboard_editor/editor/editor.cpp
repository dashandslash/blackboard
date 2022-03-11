#include "editor.h"

#include <blackboard_core/gui/components.h>
#include <blackboard_core/meta/meta.h>
#include <blackboard_core/renderer/platform/imgui_impl_sdl_bgfx.h>
#include <blackboard_core/scene/components/animation.h>
#include <blackboard_core/scene/components/name.h>
#include <blackboard_core/scene/components/selected.h>
#include <blackboard_core/scene/components/transform.h>
#include <blackboard_core/scene/components/uuid.h>
#include <blackboard_core/state/state.h>

#include <SDL/SDL.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <entt/entity/runtime_view.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <iostream>

namespace blackboard::editor {

static bool opt_fullscreen{true};
static bool opt_padding{false};
static bool show_demo_window{false};
static bool show_reflection_inspector{false};

void menu_bar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::MenuItem("Show reflection inspector", NULL, &show_reflection_inspector);
            ImGui::MenuItem("Show ImGui demo window", NULL, &show_demo_window);
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void reflection_inspector()
{
    ImGui::Begin("Reflection inspector");

    if (ImGui::CollapsingHeader("Components",
                                ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
    {
        static const auto flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
                                  ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable |
                                  ImGuiTableFlags_SortMulti;

        const auto &states = blackboard::core::get_states();
        const int num_cols = 3 + states.size();

        if (ImGui::BeginTable("Reflection_table", num_cols, flags))
        {
            ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_DefaultSort);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_DefaultSort);
            for (const auto &[state_name, _] : states)
            {
                const auto title = "State " + state_name + " count";
                ImGui::TableSetupColumn(title.data(), ImGuiTableColumnFlags_DefaultSort);
            }
            ImGui::TableHeadersRow();

            for (const auto &[type_index, info] : core::meta::get_reflected_components_infos())
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%s", std::to_string(entt::to_integral(type_index)).data());
                ImGui::TableNextColumn();
                ImGui::Text("%s", info.name.data());
                ImGui::TableNextColumn();
                ImGui::Text("%s", info.type_info.name().data());

                for (const auto &[state_name, state] : states)
                {
                    ImGui::TableNextColumn();
                    ImGui::Text(
                      "%s", std::to_string(state.m_registry.storage(type_index)->second.size()).data());
                }
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();
}

void dockspace()
{
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

    menu_bar();

    ImGui::End();

    if (show_demo_window)
    {
        ImGui::ShowDemoWindow();
    }

    if (show_reflection_inspector)
    {
        reflection_inspector();
    }
}

void entities_window(core::State &state)
{
    ImGui::Begin("Entities Window", NULL);
    static int editing{-1};
    ImGui::BeginGroup();
    ImGui::Text("Entities:");
    const auto textlineH = ImGui::GetTextLineHeight();

    ImGui::SameLine(ImGui::GetWindowWidth() - (textlineH * 2.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0.0f, 0.0f});
    if (ImGui::Button("+", {textlineH, textlineH}))
    {
        state.create_entity();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
    static entt::entity selected{entt::null};
    auto view = state.view<core::components::Name>();
    for (auto it = view.rbegin(); it != view.rend(); ++it)
    {
        auto entity = *it;
        if (state.all_of<core::components::Animation>(entity))
        {
            continue;
        }

        const auto idStr = std::to_string(entt::to_integral(entity));
        ImGui::Bullet();
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        static int editing{-1};
        const auto &name = state.get<core::components::Name>(entity);
        if (ImGui::Selectable(name.value.c_str(), selected == entity, ImGuiSelectableFlags_None))
        {
            selected = entity;
            state.m_registry.clear<core::components::Selected>();
            state.emplace_component<core::components::Selected>(entity);
            ImGui::OpenPopup((char *)&selected);
        }
    }

    ImGui::PopStyleVar();
    ImGui::EndGroup();

    if (selected != entt::null)
    {
        ImGui::SetNextWindowSize(
          {ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2.0f});
        ImGui::Begin("Properties");
        {
            // UUID
            core::gui::draw_component<core::components::Uuid>(
              state, selected, "Uuid",
              [&state](const auto &component) {
                  return core::gui::draw_component_parameter("Uuid", [&state]() {
                      return ImGui::InputText("##uuid",
                                              state.get<core::components::Uuid>(selected).get().data(),
                                              ImGuiInputTextFlags_ReadOnly);
                  });
              },
              false);

            // Name
            core::gui::draw_component<core::components::Name>(
              state, selected, "Name", [&state](core::components::Name &component) {
                  auto temp_name = component.value;

                  if (core::gui::draw_component_parameter("Name", [&state, &component, &temp_name]() {
                          return ImGui::InputText("##name", &temp_name,
                                                  ImGuiInputTextFlags_EnterReturnsTrue);
                      }))
                  {
                      state.patch<core::components::Name>(
                        selected,
                        [&state, &component, &temp_name](auto &name) { name.value = temp_name; });
                      return true;
                  }
                  return false;
              });

            auto draw_transform_fn = [&](core::components::Transform &tr) -> bool {
                auto vec3_rotation = glm::degrees(glm::eulerAngles(tr.rotation));
                auto modified =
                  core::gui::draw_component_parameter("Translation", &core::gui::vec3_control,
                                                      tr.translation, 0.0f, -180.0f, 180.0f, 150.0f);
                modified |= core::gui::draw_component_parameter(
                  "Rotation", &core::gui::vec3_control, vec3_rotation, 0.0f, -180.0f, 180.0f, 150.0f);
                modified |= core::gui::draw_component_parameter(
                  "Scale", &core::gui::vec3_control, tr.scale, 1.0f, -180.0f, 180.0f, 150.0f);
                return modified;
            };

            // Transform
            core::gui::draw_component<core::components::Transform>(state, selected, "Transform",
                                                                   draw_transform_fn);

            //             Animation
            ImGui::PushID("Animation_tr");
            //            draw_component<components::Animation<components::Transform>>(
            //              state, selected, "Animation", [&state, &draw_transform_fn](components::Animation<components::Transform> &anim) {
            //
            //                  draw_component_parameter("Current time:", [&](){
            //                      ImGui::Text("%s", std::to_string(anim.current_time()).data());
            //                      return false;
            //                  });
            //                  bool modified{false};
            //                  ImGui::Checkbox("Active", &anim.active);
            //                  ImGui::Checkbox("Loop", &anim.loop);
            //                  ImGui::Checkbox("Ping-pong", &anim.ping_pong);
            //                  if(ImGui::Button("Reset"))
            //                  {
            //                      anim.reset();
            //                      modified = true;
            //                  }
            //                  ImGui::Separator();
            //                  ImGui::Text("Start Value:");
            //                  modified = draw_transform_fn(anim.start_value());
            ////                  if (modified)
            ////                  {
            ////                      state.patch<components::Animation<components::Transform>>(
            ////                        selected, [&euler_degrees](auto &anim) {
            ////                            anim.start_value().rotation = glm::quat(glm::radians(euler_degrees));
            ////                        });
            ////                  };
            //
            //                  ImGui::Separator();
            //                  ImGui::Text("End Value:");
            //                  modified |= draw_transform_fn(anim.end_value());
            //                  ImGui::Separator();
            //
            //
            //                  return modified;
            //              });
            ImGui::PopID();
        }
        ImGui::End();
    }
    ImGui::End();
}

}    // namespace blackboard::editor
