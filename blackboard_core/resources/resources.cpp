#include "mesh.h"

#include <blackboard_core/renderer/layouts.h>
#include <blackboard_core/scene/components/uuid.h>
#include <blackboard_core/utils/zip_iterator.h>

#include <entt/core/hashed_string.hpp>
#include <entt/entity/entity.hpp>
#include <entt/resource/cache.hpp>
#include <entt/resource/loader.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <vector>

namespace blackboard::core::resources {

#include <iterator>
#include <tuple>
#include <utility>

namespace impl {
constexpr uint32_t mesh_import_flags{aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                                     aiProcess_SortByPType | aiProcess_GenNormals |
                                     aiProcess_GenUVCoords | aiProcess_JoinIdenticalVertices |
                                     aiProcess_GlobalScale | aiProcess_ValidateDataStructure};

uint32_t encode_to_rgba8(glm::vec3 &&vec)
{
    const float src[] = {vec.x * 0.5f + 0.5f, vec.y * 0.5f + 0.5f, vec.z * 0.5f + 0.5f, 0.0f};
    uint32_t dst;
    bx::packRgba8(&dst, src);
    return dst;
}

uint32_t encode_to_rgba8(glm::vec4 &&vec)
{
    const float src[] = {vec.x * 0.5f + 0.5f, vec.y * 0.5f + 0.5f, vec.z * 0.5f + 0.5f,
                         vec.w * 0.5f + 0.5f};
    uint32_t dst;
    bx::packRgba8(&dst, src);
    return dst;
}

glm::mat4 assimp_to_mat4(const aiMatrix4x4 &matrix)
{
    glm::mat4 result;
    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    result[0][0] = matrix.a1;
    result[1][0] = matrix.a2;
    result[2][0] = matrix.a3;
    result[3][0] = matrix.a4;
    result[0][1] = matrix.b1;
    result[1][1] = matrix.b2;
    result[2][1] = matrix.b3;
    result[3][1] = matrix.b4;
    result[0][2] = matrix.c1;
    result[1][2] = matrix.c2;
    result[2][2] = matrix.c3;
    result[3][2] = matrix.c4;
    result[0][3] = matrix.d1;
    result[1][3] = matrix.d2;
    result[2][3] = matrix.d3;
    result[3][3] = matrix.d4;
    return result;
}

blackboard::core::resources::Mesh processMesh(aiMesh *ai_mesh, const aiScene *ai_scene)
{
    blackboard::core::resources::Mesh mesh;
    auto &vertices = mesh.vertices;
    auto &indices = mesh.indices;
    auto &textures = mesh.textures;

    std::span<aiVector3D> ai_vertices{ai_mesh->mVertices, ai_mesh->mNumVertices};
    std::span<aiVector3D> ai_normals{ai_mesh->mNormals, ai_mesh->mNumVertices};
    std::span<aiVector3D> ai_tangents{ai_mesh->mTangents, ai_mesh->mNumVertices};
    std::span<aiVector3D> ai_bitangents{ai_mesh->mBitangents, ai_mesh->mNumVertices};
    std::span<aiVector3D *> ai_texcoords{ai_mesh->mTextureCoords, ai_mesh->mNumVertices};
    std::span<aiColor4D *> ai_colors{ai_mesh->mColors, ai_mesh->mNumVertices};

    // Vertices data
    for (auto &&[v, n, t, bt, tc, c] :
         utils::zip(ai_vertices, ai_normals, ai_tangents, ai_bitangents, ai_texcoords, ai_colors))
    {
        auto &vertex = vertices.emplace_back();

        vertex.position = glm::make_vec3(reinterpret_cast<float *>(&v));

        if (ai_mesh->HasNormals())
        {
            vertex.normal = encode_to_rgba8(glm::make_vec3(reinterpret_cast<float *>(&n)));
        }

        if (ai_mesh->HasTextureCoords(0))
        {
            vertex.uvcoords = glm::make_vec3(reinterpret_cast<float *>(&tc));
        }

        if (ai_mesh->HasTangentsAndBitangents())
        {
            vertex.tangent = encode_to_rgba8(glm::make_vec3(reinterpret_cast<float *>(&t)));
            vertex.bitangent = encode_to_rgba8(glm::make_vec3(reinterpret_cast<float *>(&bt)));
        }

        if (ai_mesh->HasVertexColors(0))
        {
            vertex.color = encode_to_rgba8(glm::make_vec4(reinterpret_cast<float *>(&c)));
        }
    }

    // Indices
    std::span<aiFace> ai_faces{ai_mesh->mFaces, ai_mesh->mNumFaces};
    for (auto &&face : ai_faces)
    {
        assert(face.mNumIndices == 3);
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }

    //Process material and texture info
    //    aiMaterial *material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
    //    textures = processTextures(material);

    return std::move(mesh);
}

void processNode(const aiNode *ai_node, const aiScene *ai_scene, Meshes &&meshes)
{
    std::span<unsigned int> ai_node_mesh_indices{ai_node->mMeshes, ai_node->mNumMeshes};
    std::span<aiMesh *> ai_meshes{ai_scene->mMeshes, ai_scene->mNumMeshes};

    for (auto &&ai_mesh_index : ai_node_mesh_indices)
    {
        aiMesh *ai_mesh = ai_meshes[ai_mesh_index];
        meshes.push_back(processMesh(ai_mesh, ai_scene));
        meshes.back().transform = assimp_to_mat4(ai_node->mTransformation);
    }

    std::span<aiNode *> ai_children_nodes{ai_node->mChildren, ai_node->mNumChildren};

    for (auto &&ai_child_node : ai_children_nodes)
    {
        processNode(ai_child_node, ai_scene, std::forward<Meshes>(meshes));
    }
}

}    // namespace impl


struct Model_loader
{
    using result_type = std::shared_ptr<Model>;
    
    result_type operator()(std::filesystem::path &&path) const
    {
        if (!std::filesystem::exists(path))
        {
            return nullptr;
        }

        auto importer = std::make_unique<Assimp::Importer>();

        auto scene = importer->ReadFile(path.string().data(), impl::mesh_import_flags);

        if (!scene || !scene->HasMeshes())
        {
            return nullptr;
        }

        auto model = std::make_shared<Model>();
        impl::processNode(scene->mRootNode, scene, std::forward<Meshes>(model->meshes));

        return model;
    }
};

entt::resource_cache<Model, Model_loader> models_cache;
    
entt::id_type load_model(std::filesystem::path &&path)
{
    const auto key = entt::hashed_string(core::components::Uuid().get().data());
    if (models_cache.force_load(key, std::forward<std::filesystem::path>(path)).second)
    {
        return key;
    }

    return entt::null;
}

std::shared_ptr<Model> get_model(const entt::id_type key)
{
    if(models_cache.contains(key))
    {
       return models_cache[key].handle();
    }
    return nullptr;
}

bool is_valid_model_key(const entt::id_type key)
{
    return key != entt::null && models_cache.contains(key);
}

}   // namespace blackboard::core::resources
