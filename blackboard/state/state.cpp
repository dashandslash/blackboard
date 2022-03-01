#include "state.h"

#include "meta/meta.h"
#include "scene/components/animation.h"
#include "scene/components/name.h"
#include "scene/components/transform.h"
#include "scene/components/uuid.h"

namespace blackboard {

std::unordered_map<std::string, State> states;

State::State()
{
    meta::register_component<components::Transform>("Transform");
    meta::register_component<components::Animation>("Animation");
    meta::register_component<components::Uuid>("Uuid");
    meta::register_component<components::Name>("Name");
}

entt::entity State::create_entity()
{
    const auto entity = m_registry.create();
    m_registry.emplace<components::Uuid>(entity);
    m_registry.emplace<components::Name>(entity, "Name_" + std::to_string(entt::to_integral(entity)));
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

std::unordered_map<std::string, State> &get_states()
{
    return states;
}

}    // namespace blackboard
