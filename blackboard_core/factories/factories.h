#pragma once
#include "scene/components/animation.h"
#include "state/state.h"

namespace blackboard::core::factories {

template<typename T>
void create_animation(State &state, entt::entity entity, T &&from, T &&to,
                      components::Animation::Info &&info)
{
    auto animation_entity = state.create_entity();
    state.emplace_component<components::Animation>(animation_entity, state.get<T>(entity),
                                                   std::move(from), std::move(to), std::move(info));

    if (!state.all_of<components::Animation_entities>(entity))
    {
        state.emplace_component<components::Animation_entities>(entity);
    }

    state.patch<components::Animation_entities>(
      entity, [&animation_entity](components::Animation_entities &comp) {
          comp.entities.push_back(animation_entity);
      });
}

}    // namespace blackboard::core::factories
