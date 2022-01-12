#pragma once
#include "material.h"

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

inline renderer::material::Manager &material_manager()
{
    static renderer::material::Manager mm;
    return mm;
}
}    // namespace renderer
}    // namespace blackboard
