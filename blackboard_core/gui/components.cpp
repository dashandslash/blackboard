#include "components.h"

#include "scene/components/transform.h"

#include <string>

namespace blackboard::core::gui {

bool vec3_control(glm::vec3 &&values, const float resetValue, const float min, const float max,
                  const float columnWidth)
{
    bool modified{false};
    float lineHeight{ImGui::GetTextLineHeight()};
    ImVec2 buttonSize{lineHeight, lineHeight + ImGui::GetStyle().FramePadding.y};
    ImGui::PushStyleColor(ImGuiCol_Button, {0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.9f, 0.2f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0.8f, 0.1f, 0.15f, 1.0f});
    if (ImGui::Button("X", buttonSize))
    {
        values.x = resetValue;
        modified = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::SetNextItemWidth((ImGui::GetContentRegionAvail().x - buttonSize.x * 2.0f) / 3.0f);
    if (ImGui::DragFloat("##X", &values.x, 0.1f, min, max, "%.2f"))
    {
        modified = true;
    }
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, {0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0.2f, 0.7f, 0.2f, 1.0f});
    if (ImGui::Button("Y", buttonSize))
    {
        values.y = resetValue;
        modified = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::SetNextItemWidth((ImGui::GetContentRegionAvail().x - buttonSize.x) / 2.0f);
    if (ImGui::DragFloat("##Y", &values.y, 0.1f, min, max, "%.2f"))
    {
        modified = true;
    }
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, {0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.2f, 0.35f, 0.9f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0.1f, 0.25f, 0.8f, 1.0f});
    if (ImGui::Button("Z", buttonSize))
    {
        values.z = resetValue;
        modified = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::SetNextItemWidth((ImGui::GetContentRegionAvail().x));
    if (ImGui::DragFloat("##Z", &values.z, 0.1f, min, max, "%.2f"))
    {
        modified = true;
    }
    return modified;
}

}    // namespace blackboard::core::gui
