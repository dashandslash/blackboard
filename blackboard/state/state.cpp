#include "state.h"

#include "meta/meta.h"
#include "scene/components/animation.h"
#include "scene/components/name.h"
#include "scene/components/transform.h"
#include "scene/components/uuid.h"

namespace blackboard {

State::State()
{
    meta::register_component<components::Transform>("Transform");
    meta::register_component<components::Animation<components::Transform>>("Animation<Transform>");
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

}    // namespace blackboard
