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

using Meta_infos = entt::dense_map<entt::id_type, Info>;

inline constexpr auto property_name_key = entt::hashed_string("name");

namespace impl {

inline Meta_infos meta_components_infos;

template<typename T>
inline void fn(std::string &&reflected_name)
{
    const entt::type_info type_info = entt::type_id<T>();

    if (meta_components_infos.contains(type_info.hash()))
    {
        return;
    }

    entt::meta<T>()
      .type(entt::hashed_string(reflected_name.c_str()))
      .prop(property_name_key, reflected_name);

    // To ensure the property has been associated
    if (auto prop = entt::resolve<T>().prop(property_name_key); prop)
    {
        meta_components_infos.insert(
          {type_info.hash(),
           {.name = prop.value().template cast<std::string>(), .type_info = type_info}});
    }
}
}    // namespace impl

template<typename T>
inline void register_component(std::string &&reflected_name)
{
    using namespace entt::literals;
    using namespace impl;

    impl::fn<T>(std::move(reflected_name));
}

const Meta_infos &get_reflected_components_infos();

}    // namespace blackboard::meta
