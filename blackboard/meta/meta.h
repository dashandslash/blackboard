#pragma once
#include <entt/core/hashed_string.hpp>
#include <entt/entt.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/resolve.hpp>

#include <iostream>
#include <string>

namespace blackboard::meta {

struct info
{
    const std::string type_name{};
    const std::string name{};
};

namespace impl {
inline constexpr auto name_hs = entt::hashed_string("name");
inline std::vector<info> reflected_components_infos;
}    // namespace impl

template<typename T>
inline void register_component(std::string &&reflected_name)
{
    using namespace entt::literals;
    using namespace impl;

    if (entt::resolve(entt::type_id<T>()))
    {
        return;
    }

    entt::meta<T>().type(entt::hashed_string(reflected_name.c_str())).prop(name_hs, reflected_name);
    const entt::meta_type meta_type = entt::resolve<T>();

    if (auto prop = meta_type.prop(name_hs); prop)
    {
        reflected_components_infos.emplace_back(
          info{.type_name = std::string{meta_type.info().name()},
               .name = prop.value().template cast<std::string>()});
    }
}

const std::vector<info> &get_reflected_components_infos();

}    // namespace blackboard::meta
