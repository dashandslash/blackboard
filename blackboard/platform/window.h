#pragma once

#include <string>

namespace blackboard {

template<typename WindowType>
struct Window
{
    void init_platform_window();

    std::string title{"title"};
    uint16_t width{1280u};
    uint16_t height{720u};
    uint16_t imgui_view_id{255};    // might be possible to remove this id
    bool fullscreen{false};
    WindowType *window{nullptr};
    bool is_dragging{false};

    int prev_mouse_x{0};
    int prev_mouse_y{0};
    int mouse_x{0};
    int mouse_y{0};

    int window_x{0};
    int window_y{0};
};

}    // namespace blackboard
