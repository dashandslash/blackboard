#pragma once
#include <blackboard_core/resources/mesh.h>

#include <entt/resource/handle.hpp>

#include <filesystem>

namespace blackboard::core::resources {

std::filesystem::path path();

void init(std::filesystem::path &&app_base_path);

entt::id_type load_model(std::filesystem::path &&path);

Model &get_model_ref(const entt::id_type identifier);
entt::resource_handle<Model> get_model_handle(const entt::id_type identifier);

bool is_valid_model_key(const entt::id_type identifier);

}    // namespace blackboard::core::resources
