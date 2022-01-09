#pragma once

#include <string>

struct SDL_Window;

namespace blackboard {
namespace renderer {
bool init(SDL_Window *window, const uint16_t width, const uint16_t height);
}
}    // namespace blackboard
