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

struct Vertex
{
    glm::vec3 position{};
    uint32_t normal{};
    uint32_t tangent{};
    uint32_t bitangent{};
    uint32_t color{0xff0000ff};
    glm::vec2 uvcoords{};
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<uint32_t> textures;

    glm::mat4 transform;

    bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;
};

using Meshes = std::vector<Mesh>;

struct Model
{
    //    Model(const std::filesystem::path &path);
    Meshes meshes;
};

}    // namespace blackboard::core::resources
