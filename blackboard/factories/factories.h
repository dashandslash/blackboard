#pragma once
#include "scene/components/animation.h"
#include "state/state.h"

namespace blackboard::factories {

template<typename T>
void create_animation(State &state, entt::entity entity, T &&from, T &&to,
                      components::Animation::Info &&info)
{
    auto animation_entity = state.create_entity();
    state.emplace_component<components::Animation>(animation_entity, state.m_registry.get<T>(entity),
                                                   std::move(from), std::move(to), std::move(info));

    if (!state.m_registry.all_of<components::Animation_entities>(entity))
    {
        state.emplace_component<components::Animation_entities>(entity);
    }

    state.patch<components::Animation_entities>(
      entity, [&animation_entity](components::Animation_entities &comp) {
          comp.entities.push_back(animation_entity);
      });
}

}    // namespace blackboard::factories
