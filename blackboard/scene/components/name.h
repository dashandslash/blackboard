#pragma once

#ifdef _WIN32
// todo!
#elif __APPLE__
#include <uuid/uuid.h>
#endif

namespace blackboard::components {

struct Name
{
    std::string value{};
};

}    // namespace blackboard::components
