#pragma once
#include <entt/core/hashed_string.hpp>
#include <entt/entt.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/resolve.hpp>

#include <iostream>
#include <string>

struct SDL_Window;

namespace blackboard {
struct State
{
    State();

    using entity_type = entt::registry::entity_type;

    entt::entity create_entity();

    template<typename Component, typename... Args>
    decltype(auto) emplace_component(const entity_type entity, Args &&... args)
    {
        return m_registry.emplace_or_replace<Component>(entity, std::forward<Args>(args)...);
    }

    template<typename Component, typename... Func>
    decltype(auto) patch(const entity_type entity, Func &&... func)
    {
        return m_registry.patch(entity, std::forward<Func>(func)...);
    }

    template<typename Component, typename... Other, typename... Exclude>
    [[nodiscard]] decltype(auto) view(entt::exclude_t<Exclude...> exc = {}) const
    {
        return m_registry.view<Component, Other..., Exclude...>(
          std::forward<entt::exclude_t<Exclude...>>(exc));
    }

    template<typename Component, typename... Other, typename... Exclude>
    [[nodiscard]] decltype(auto) view(entt::exclude_t<Exclude...> exc = {})
    {
        return m_registry.view<Component, Other..., Exclude...>(
          std::forward<entt::exclude_t<Exclude...>>(exc));
    }

private:
    entt::registry m_registry;
};
}    // namespace blackboard
