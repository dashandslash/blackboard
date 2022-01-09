#pragma once

#include <string>

struct SDL_Window;

namespace blackboard {
namespace renderer {

enum class Api : uint8_t
{
    none = 0,
    metal,
    d3d11
};

bool init(SDL_Window *window, const uint16_t width, const uint16_t height);
}    // namespace renderer
}    // namespace blackboard
