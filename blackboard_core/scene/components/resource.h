#pragma once
#include <resources/resources.h>

#include <entt/core/fwd.hpp>

namespace blackboard::core::components {

template<typename T>
struct Resource_key
{
    entt::id_type value;

    operator const entt::id_type &() const
    {
        return value;
    }
};

using model_resource_key = Resource_key<resources::Model>;

}    // namespace blackboard::core::components
