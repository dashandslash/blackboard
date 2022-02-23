#pragma once
#include "state/state.h"

#include <glm/fwd.hpp>
#include <imgui/imgui.h>

#include <string>

namespace blackboard::gui {

struct PushStyleCompact
{
    PushStyleCompact()
    {
        ImGuiStyle &style = ImGui::GetStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                            ImVec2(style.FramePadding.x, (float)(int)(style.FramePadding.y * 0.60f)));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                            ImVec2(style.ItemSpacing.x, (float)(int)(style.ItemSpacing.y * 0.60f)));
    }
    ~PushStyleCompact()
    {
        ImGui::PopStyleVar(2);
    }
};

bool vec3_control(glm::vec3 &&values, const float resetValue = 0.0f, const float columnWidth = 150.0f);

template<typename UIFunction, typename... Args>
static bool show_component_parameter(const std::string &label, UIFunction &&uiFunction, Args &&... args)
{
    bool modified{false};
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 2});
    if (ImGui::BeginTable("##table", 2))
    {
        ImGui::TableSetupColumn("header1", ImGuiTableColumnFlags_WidthFixed,
                                150.0f);    // Default to 100.0f
        ImGui::SetNextItemWidth(150.0f);
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
static bool show_component(blackboard::State &state, const entt::entity entity,
                           const std::string &label, UIFunction uiFunction)
{
    PushStyleCompact compact;
    bool modified{false};
    if (state.all_of<T>(entity))
    {
        auto typeName = entt::type_name<T>::value();
        if (ImGui::CollapsingHeader(
              std::string{typeName.substr(typeName.find_last_of("::") + 1)}.c_str(),
              ImGuiTreeNodeFlags_Framed))
        {
            const auto topLeft = ImGui::GetItemRectMin();
            const auto topRight = ImVec2{ImGui::GetItemRectMax().x, topLeft.y};
            ImGui::BeginGroup();

            auto &component = state.get<T>(entity);
            modified = uiFunction(component);

            ImGui::EndGroup();
            const auto bottomRight = ImVec2{topRight.x, ImGui::GetItemRectMax().y};
            const auto bottomLeft = ImVec2{topLeft.x, bottomRight.y};

            const auto borderCol = ImGui::GetStyle().Colors[ImGuiCol_Border];
            const auto borderColInt =
              IM_COL32(borderCol.x * 255, borderCol.y * 255, borderCol.z * 255, borderCol.w * 255);
            auto &dl = *ImGui::GetWindowDrawList();
            dl.AddLine(topLeft, bottomLeft, borderColInt);
            dl.AddLine(bottomLeft, bottomRight, borderColInt);
            dl.AddLine(topRight, bottomRight, borderColInt);
        }
    }
    return modified;
}

}    // namespace blackboard::gui
