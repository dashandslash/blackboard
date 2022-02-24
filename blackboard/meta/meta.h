#pragma once
#include <entt/core/hashed_string.hpp>
#include <entt/entt.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/resolve.hpp>

#include <iostream>
#include <string>

namespace blackboard::meta {

struct Info
{
    const std::string name{};
    const entt::type_info type_info;
};

using Meta_infos = std::unordered_map<entt::id_type, Info>;

namespace impl {
inline constexpr auto name_hs = entt::hashed_string("name");
inline Meta_infos meta_components_infos;
}    // namespace impl

template<typename T>
inline void register_component(std::string &&reflected_name)
{
    using namespace entt::literals;
    using namespace impl;
    const entt::type_info type_info = entt::type_id<T>();

    if (meta_components_infos.contains(type_info.hash()))
    {
        return;
    }

    entt::meta<T>().type(entt::hashed_string(reflected_name.c_str())).prop(name_hs, reflected_name);
    const entt::meta_type meta_type = entt::resolve<T>();

    if (auto prop = meta_type.prop(name_hs); prop)
    {
        meta_components_infos.insert({type_info.hash(),
                                      {.name = prop.value().template cast<std::string>(),
                                       .type_info = type_info}});
    }
}

const Meta_infos &get_reflected_components_infos();

}    // namespace blackboard::meta
