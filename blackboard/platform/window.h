#pragma once

#include <string>

struct SDL_Window;

namespace blackboard {

struct Window
{
    void init_platform_window();

    std::pair<uint16_t, uint16_t> get_size_in_pixels() const;

    // get position
    std::pair<uint16_t, uint16_t> get_position() const;

    std::string title{"title"};
    uint16_t width{1280u};
    uint16_t height{720u};
    uint16_t imgui_view_id{255};    // might be possible to remove this id
    bool fullscreen{false};
    SDL_Window *window{nullptr};
    bool is_dragging{false};
};

}    // namespace blackboard
