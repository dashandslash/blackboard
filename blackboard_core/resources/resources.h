#pragma once
#include <blackboard_core/resources/mesh.h>

#include <filesystem>

namespace blackboard::core::resources {

entt::id_type load_model(std::filesystem::path &&path);

std::shared_ptr<Model> get_model(const entt::id_type identifier);

bool is_valid_model_key(const entt::id_type identifier);

}    // namespace blackboard::core::resources
