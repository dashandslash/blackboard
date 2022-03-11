#pragma once
#include <bgfx/bgfx.h>
#include <imgui/imgui.h>

#include <filesystem>
#include <string>

namespace blackboard {
namespace core {
struct State;
}
namespace editor {

void dockspace();

void entities_window(core::State &state);

}    // namespace editor
}    // namespace blackboard
