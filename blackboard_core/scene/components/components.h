#pragma once
#include "name.h"
#include "uuid.h"
#include "transform.h"
#include "resource.h"
#include "selected.h"
#include <resources/resources.h>

#include <entt/core/type_traits.hpp>


namespace blackboard::core::components
{
const auto types = std::tuple<Name, Uuid, Transform, Selected, Resource_key<resources::Mesh>>();
}
