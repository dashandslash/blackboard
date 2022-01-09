#pragma once

#include <memory>
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
};

void init();
}    // namespace blackboard::gui
