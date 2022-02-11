#pragma once
#include <SDL.h>

#include <filesystem>

namespace blackboard::resources {

inline std::filesystem::path path()
{
#ifndef __EMSCRIPTEN__
    if (char *base_path = SDL_GetBasePath(); base_path)
    {
        return {base_path};
    }
#endif
    return strdup("/");
}
}    // namespace blackboard::resources
