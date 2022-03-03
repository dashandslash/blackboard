#pragma once
#include "state/state.h"

#include <glm/fwd.hpp>
#include <imgui/imgui.h>

#include <string>

namespace blackboard::gui {

struct push_style_compact
{
    push_style_compact()
    {
        ImGuiStyle &style = ImGui::GetStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                            ImVec2(style.FramePadding.x, (float)(int)(style.FramePadding.y * 0.60f)));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                            ImVec2(style.ItemSpacing.x, (float)(int)(style.ItemSpacing.y * 0.60f)));
    }
    ~push_style_compact()
    {
        ImGui::PopStyleVar(2);
    }
};

bool vec3_control(glm::vec3 &&values, const float resetValue = 0.0f, const float min = 0.0f,
                  const float max = 0.0f, const float columnWidth = 150.0f);

template<typename UIFunction, typename... Args>
static bool draw_component_parameter(const std::string &label, UIFunction &&uiFunction, Args &&... args)
{
    bool modified{false};
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 2});
    static const auto flags =
      ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
    if (ImGui::BeginTable("##table", 2, flags))
    {
        ImGui::TableSetupColumn("header1", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableNextColumn();
        ImGui::Text("%s", label.c_str());
        ImGui::TableNextColumn();

        ImGui::SetNextItemWidth(-1.0f);
        ImGui::PushID(label.c_str());
        modified = uiFunction(std::move(std::forward<Args>(args))...);
        ImGui::PopID();
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
    return modified;
}

template<typename T, typename UIFunction>
static bool draw_component(blackboard::State &state, const entt::entity entity,
                           const std::string &label, UIFunction &&ui_function,
                           const bool with_header = true)
{
    push_style_compact compact;

    auto fn = [&state, &entity, &ui_function]() -> bool {
        ImGui::BeginGroup();
        T &component = state.get<T>(entity);
        bool modified = ui_function(component);
        ImGui::EndGroup();
        return modified;
    };

    if (state.all_of<T>(entity))
    {
        if (!with_header)
        {
            return fn();
        }

        if (ImGui::CollapsingHeader(label.c_str(),
                                    ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
        {
            return fn();
        }
    }
    return false;
}

}    // namespace blackboard::gui
