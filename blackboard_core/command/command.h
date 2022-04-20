#pragma once

#include <state/state.h>

#include <entt/entt.hpp>

#include <functional>

namespace blackboard::core::cmd {

inline std::vector<entt::meta_any> queue;

template <typename Comp>
struct Replace
{
    entt::entity entity{entt::null};
    Comp component;
};

template <typename T>
void replace_component(State &state, const T &arg)
{
    state.emplace_component<decltype(arg.component)>(arg.entity, arg.component);
};

struct a_command
{};

template <typename T>
void execute(State&, const T&);

template <typename T>
void register_component_to_execute()
{
    using namespace entt::literals;
    entt::meta<T>().type(entt::hashed_string(entt::type_id<T>().name().data())).template func<execute<T>>("cmd::execute"_hs);
}

template <typename T>
void register_component_to_replace()
{
    using namespace entt::literals;
    entt::meta<Replace<T>>().type(entt::hashed_string{entt::type_id<Replace<T>>().name().data()}).template func<replace_component<Replace<T>>>("cmd::replace_component"_hs);
}

template<typename... Args>
void register_components_to_replace(const std::tuple<Args...> &)
{
    (..., cmd::register_component_to_replace<Args>());
}

void init(State &state);

inline void dispatch(State &&state)
{
    using namespace entt::literals;
    while (!queue.empty())
    {
        auto &&cmd = queue.back();
        auto type = entt::resolve(entt::hashed_string(cmd.type().info().name().data()));
        if(!type)
        {
            std::cout << "Command type not resolved " << cmd.type().info().name() << std::endl;
            return;
        }
        
        for (auto f: type.func())
        {
            f.invoke({}, entt::forward_as_meta(state), cmd);
        }
        queue.pop_back();
    }
}

}    // namespace blackboard::core
