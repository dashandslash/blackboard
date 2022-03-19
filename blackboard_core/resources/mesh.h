#pragma once
#include <assimp/material.h>
#include <assimp/pbrmaterial.h>
#include <bgfx/bgfx.h>
#include <bimg/bimg.h>
#include <entt/container/dense_map.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <blackboard_core/renderer/shaders/definitions.sh>

#include <array>
#include <filesystem>
#include <unordered_map>
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

using Image = bimg::ImageContainer;
using Texture_type = uint8_t;

inline static const std::unordered_map<Texture_type, std::string> Texture_type_names{
  {SAMPLER2D_TEXTURE0_STAGE, SAMPLER2D_TEXTURE0_NAME},
  {SAMPLER2D_TEXTURE1_STAGE, SAMPLER2D_TEXTURE1_NAME},
  {SAMPLER2D_TEXTURE2_STAGE, SAMPLER2D_TEXTURE2_NAME},
  {SAMPLER2D_TEXTURE3_STAGE, SAMPLER2D_TEXTURE3_NAME},
  {SAMPLER2D_ALBEDO_STAGE, SAMPLER2D_ALBEDO_NAME},
  {SAMPLER2D_DIFFUSE_STAGE, SAMPLER2D_DIFFUSE_NAME},
  {SAMPLER2D_NORMALS_STAGE, SAMPLER2D_NORMALS_NAME},
  {SAMPLER2D_AO_STAGE, SAMPLER2D_AO_NAME},
  {SAMPLER2D_EMISSIVE_STAGE, SAMPLER2D_EMISSIVE_NAME},
  {SAMPLER2D_ROUGH_METALLIC_STAGE, SAMPLER2D_ROUGH_METALLIC_NAME},
  {SAMPLER2D_HEIGHT_STAGE, SAMPLER2D_HEIGHT_NAME},
  {SAMPLER2D_OPACITY_STAGE, SAMPLER2D_OPACITY_NAME},
  {SAMPLER2D_SPECULAR_STAGE, SAMPLER2D_SPECULAR_NAME}};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    entt::dense_map<Texture_type, Image> images;

    glm::mat4 transform;

    bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;
    entt::dense_map<Texture_type, bgfx::TextureHandle> textures;
};

using Meshes = std::vector<Mesh>;

struct Model
{
    Model() = delete;
    Model(const std::filesystem::path &path): m_file_path{path}
    {}
    Meshes meshes;

    const std::filesystem::path &file_path() const
    {
        return m_file_path;
    }

    std::filesystem::path file_name() const
    {
        return m_file_path.filename();
    }

    std::filesystem::path path() const
    {
        return m_file_path.parent_path();
    }

private:
    std::filesystem::path m_file_path{};
};

}    // namespace blackboard::core::resources
