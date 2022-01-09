#pragma once

#include <memory>
#include <string>

struct SDL_Window;

namespace blackboard::gui {

template<typename WindowType>
struct Window
{
    uint16_t width{1280u};
    uint16_t height{720u};
    std::string title{"title"};
    uint16_t imgui_view_id{255};
    WindowType *window;
};

void init();
}    // namespace blackboard::gui
