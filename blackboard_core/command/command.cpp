#include "command.h"


#include <algorithm>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <entt/core/hashed_string.hpp>
#include <entt/core/type_info.hpp>
#include <entt/core/utility.hpp>
#include <entt/meta/container.hpp>
#include <entt/meta/ctx.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/pointer.hpp>
#include <entt/meta/resolve.hpp>
#include <entt/meta/template.hpp>

#include <scene/components/name.h>
#include <scene/components/components.h>
#include <state/state.h>

#include <vector>

namespace blackboard::core::cmd {

std::vector<std::reference_wrapper<State>> states;

void init(State &state)
{
    states.push_back(state);
    register_components_to_replace(components::types);
}

}    // namespace blackboard::core
