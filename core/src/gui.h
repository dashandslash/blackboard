#pragma once

#include <memory>
#include <string>

struct SDL_Window;

namespace blackboard
{
namespace gui
{

struct Window_data
{
    std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>> main_window;
    uint16_t width{1280u};
    uint16_t height{720u};
};

Window_data init(const std::string &window_title) noexcept;
}
}
