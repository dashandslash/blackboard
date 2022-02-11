#pragma once
#include <SDL.h>

#include <filesystem>

namespace blackboard::resources {

inline std::filesystem::path path()
{
    if (char *base_path = SDL_GetBasePath(); base_path)
    {
        return {base_path};
    }
    return strdup("/");
}
}    // namespace blackboard::resources
