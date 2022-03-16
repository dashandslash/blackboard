#pragma once
#include "scene/components/animation.h"
#include "scene/components/name.h"
#include "scene/components/resource.h"
#include "scene/components/selected.h"
#include "scene/components/transform.h"
#include "scene/components/uuid.h"

#include <entt/core/hashed_string.hpp>
#include <entt/entt.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/resolve.hpp>

#include <iostream>
#include <string>

namespace blackboard::core::meta {

struct Info
{
    const std::string name{};
    const entt::type_info type_info;
};

using Meta_infos = std::unordered_map<entt::id_type, Info>;

inline constexpr auto property_name_key = entt::hashed_string("name");

namespace impl {

template<typename T>
inline std::pair<const entt::id_type, Info> create_meta(std::string &&reflected_name)
{
    entt::meta<T>()
      .type(entt::hashed_string(reflected_name.c_str()))
      .prop(property_name_key, reflected_name);

    const entt::type_info type_info = entt::type_id<T>();
    return {type_info.hash(), {.name = reflected_name, .type_info = type_info}};
}

inline Meta_infos meta_components_infos = {
  meta::impl::create_meta<components::Uuid>("Uuid"),
  meta::impl::create_meta<components::Name>("Name"),
  meta::impl::create_meta<components::Transform>("Transform"),
  meta::impl::create_meta<components::Animation>("Animation"),
  meta::impl::create_meta<components::model_resource_key>("Model Resource Key"),
  meta::impl::create_meta<components::Selected>("Selected")};
}    // namespace impl

template<typename T>
inline void register_component(std::string &&reflected_name)
{
    using namespace impl;

    if (!meta_components_infos.contains(entt::type_id<T>().hash()))
    {
        meta_components_infos.insert(impl::create_meta<T>(std::move(reflected_name)));
    }
}

const Meta_infos &get_reflected_components_infos();

}    // namespace blackboard::core::meta
