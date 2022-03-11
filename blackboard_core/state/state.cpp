#include "state.h"

#include "meta/meta.h"
#include "scene/components/animation.h"
#include "scene/components/name.h"
#include "scene/components/transform.h"
#include "scene/components/uuid.h"

#include <entt/entt.hpp>

namespace blackboard::core {

entt::dense_map<std::string, State> states;

State::State()
{}

entt::entity State::create_entity(std::string &&name)
{
    const auto entity = m_registry.create();
    m_registry.emplace<components::Uuid>(entity);
    if (name.empty())
    {
        name = "Name_" + std::to_string(entt::to_integral(entity));
    }
    m_registry.emplace<components::Name>(entity, name);
    return entity;
}

State &create_state(const std::string &state_name)
{
    assert(!states.contains(state_name));
    auto entry = states.insert({state_name, State()});
    return entry.first->second;
}

State &get_state(const std::string &state_name)
{
    assert(states.contains(state_name));
    return states[state_name];
}

entt::dense_map<std::string, State> &get_states()
{
    return states;
}

}    // namespace blackboard::core
