#pragma once

#include <bgfx/bgfx.h>
#include <imgui/imgui.h>

#include <filesystem>
#include <string>

namespace blackboard::gui {

void init();

void set_dracula_theme();

void dockspace();

/// @brief Load a font inside the collection of gui fonts
void load_font(const std::filesystem::path &path, const float size, const bool set_as_default = false);

// input format #aa11ff
ImVec4 string_hex_to_rgb_float(const std::string &color);

// bgfx texture handle to imgui texture id
ImTextureID toId(bgfx::TextureHandle _handle, uint8_t _flags, uint8_t _mip);

}    // namespace blackboard::gui
