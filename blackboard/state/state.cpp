#include "state.h"

#include "meta/meta.h"
#include "scene/components/animation.h"
#include "scene/components/transform.h"

namespace blackboard {

State::State()
{
    meta::register_component<components::Transform>("Transform");
    meta::register_component<components::Animation<components::Transform>>("Animation<Transform>");
}

entt::entity State::create_entity()
{
    return m_registry.create();
}

}    // namespace blackboard
