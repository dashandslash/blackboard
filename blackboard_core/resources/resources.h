#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <filesystem>
#include <vector>

namespace blackboard::core::resources {

entt::id_type load_model(std::filesystem::path &&path);

Model &get_model_ref(const entt::id_type identifier);
entt::resource_handle<Model> get_model_handle(const entt::id_type identifier);

bool is_valid_model_key(const entt::id_type identifier);

}    // namespace blackboard::core::resources
