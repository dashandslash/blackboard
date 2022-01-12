#pragma once
#include <SDL.h>

#include <filesystem>

namespace blackboard::resources {

inline std::filesystem::path path()
{
    char *base_path = SDL_GetBasePath();
    if (base_path)
    {
        return {SDL_GetBasePath()};
    }

    return {"./"};
}
}    // namespace blackboard::resources
