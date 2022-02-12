#pragma once

#include <bgfx/bgfx.h>
#include <imgui/imgui.h>

#include <string>

namespace blackboard::gui {

void init();

void set_dracula_theme();

void dockspace();

// input format #aa11ff
ImVec4 string_hex_to_rgb_float(const std::string &color);

// bgfx texture handle to imgui texture id
ImTextureID toId(bgfx::TextureHandle _handle, uint8_t _flags, uint8_t _mip);

}    // namespace blackboard::gui
