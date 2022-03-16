#pragma once
#include <entt/container/fwd.hpp>
#include <entt/entity/registry.hpp>

#include <iostream>
#include <string>

struct SDL_Window;

namespace blackboard::core {

struct State
{
    State();

    using entity_type = entt::registry::entity_type;

    entt::entity create_entity(std::string &&name = "");

    template<typename Component, typename... Args>
    decltype(auto) emplace_component(const entity_type entity, Args &&... args)
    {
        return m_registry.emplace_or_replace<Component>(entity, std::forward<Args>(args)...);
    }

    template<typename Component, typename... Func>
    decltype(auto) patch(const entity_type entity, Func &&... func)
    {
        return m_registry.patch<Component>(entity, std::forward<Func>(func)...);
    }

    template<typename Component, typename... Other, typename... Exclude>
    [[nodiscard]] decltype(auto) view(entt::exclude_t<Exclude...> args = {}) const
    {
        return m_registry.view<Component, Other...>(std::forward<entt::exclude_t<Exclude...>>({}));
    }

    template<typename Component, typename... Other, typename... Exclude>
    [[nodiscard]] decltype(auto) view(entt::exclude_t<Exclude...> args = {})
    {
        return m_registry.view<Component, Other...>(std::forward<entt::exclude_t<Exclude...>>({}));
    }

    template<typename... Args>
    auto all_of(const entity_type entity)
    {
        return m_registry.all_of<Args...>(entity);
    }

    template<typename... Args>
    decltype(auto) get(const entity_type entity)
    {
        return m_registry.get<Args...>(entity);
    }

    template<typename... Args>
    decltype(auto) get(const entity_type entity) const
    {
        return m_registry.get<Args...>(entity);
    }

    template<typename Func>
    void each(Func &&func) const
    {
        m_registry.each(std::forward<Func>(func));
    }

    decltype(auto) storage(const entt::id_type id) const
    {
        return m_registry.storage(id);
    }

    template<typename... T>
    decltype(auto) clear()
    {
        return m_registry.clear<T...>();
    }

    //private:
    entt::registry m_registry;
};

State &create_state(const std::string &state_name);
State &get_state(const std::string &state_name);
entt::dense_map<std::string, State> &get_states();

}    // namespace blackboard::core
