#pragma once

#include <bgfx/bgfx.h>
#include <imgui/imgui.h>

#include <string>

namespace blackboard::gui {

template<typename WindowType>
struct Window
{
    uint16_t width{1280u};
    uint16_t height{720u};
    std::string title{"title"};
    uint16_t imgui_view_id{255};    // might be possible to remove this id
    bool fullscreen{false};
    WindowType *window;
    bool is_dragging{false};

    int prev_mouse_x;
    int prev_mouse_y;
    int mouse_x;
    int mouse_y;

    int window_x;
    int window_y;
};

void init();

void set_dracula_theme();

void dockspace();

// input format #aa11ff
ImVec4 string_hex_to_rgb_float(const std::string &color);

ImTextureID toId(bgfx::TextureHandle _handle, uint8_t _flags, uint8_t _mip);

}    // namespace blackboard::gui
