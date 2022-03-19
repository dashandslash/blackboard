#include "mesh.h"

#include <blackboard_core/meta/meta.h>
#include <blackboard_core/renderer/layouts.h>
#include <blackboard_core/resources/mesh.h>
#include <blackboard_core/scene/components/uuid.h>
#include <blackboard_core/utils/zip_iterator.h>

#include <assimp/Importer.hpp>
#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <bimg/decode.h>
#include <bx/allocator.h>
#include <bx/error.h>
#include <bx/file.h>
#include <entt/core/hashed_string.hpp>
#include <entt/entity/entity.hpp>
#include <entt/resource/cache.hpp>
#include <entt/resource/handle.hpp>
#include <entt/resource/loader.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <entt/core/hashed_string.hpp>
#include <entt/core/type_info.hpp>
#include <entt/core/utility.hpp>
#include <entt/meta/container.hpp>
#include <entt/meta/ctx.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/pointer.hpp>
#include <entt/meta/resolve.hpp>
#include <entt/meta/template.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
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

bx::AllocatorI *getDefaultAllocator()
{
    BX_PRAGMA_DIAGNOSTIC_PUSH();
    BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(
      4459);    // warning C4459: declaration of 's_allocator' hides global declaration
    BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wshadow");
    static bx::DefaultAllocator s_allocator;
    return &s_allocator;
    BX_PRAGMA_DIAGNOSTIC_POP();
}

bool image_load(const std::filesystem::path &&filePath, Image &&image_container)
{
    bx::Error error;
    bx::FileReader reader;
    auto ret = reader.open(filePath.string().data(), &error);
    uint32_t fileSize = (uint32_t)bx::getSize(&reader);
    const bgfx::Memory *mem = bgfx::alloc(static_cast<uint32_t>(fileSize));
    ret = reader.read(mem->data, fileSize, &error);
    reader.close();
    const auto temp_image_container = bimg::imageParse(impl::getDefaultAllocator(), mem->data,
                                                       mem->size, bimg::TextureFormat::Count, &error);
    if (!temp_image_container)
    {
        // log error
        return false;
    }
    image_container = std::move(*temp_image_container);
    return true;
}

uint8_t get_texture_stage(const aiTextureType ai_texture_type)
{
    switch (ai_texture_type)
    {
        case aiTextureType_DIFFUSE:
            return SAMPLER2D_DIFFUSE_STAGE;
        case aiTextureType_BASE_COLOR:
            return SAMPLER2D_ALBEDO_STAGE;
        case aiTextureType_NORMALS:
            return SAMPLER2D_NORMALS_STAGE;
        case aiTextureType_AMBIENT_OCCLUSION:
        case aiTextureType_LIGHTMAP:
        case aiTextureType_AMBIENT:
            return SAMPLER2D_AO_STAGE;
        case aiTextureType_UNKNOWN:
            return SAMPLER2D_ROUGH_METALLIC_STAGE;
        case aiTextureType_EMISSIVE:
            return SAMPLER2D_EMISSIVE_STAGE;
        case aiTextureType_HEIGHT:
            return SAMPLER2D_HEIGHT_STAGE;
        case aiTextureType_OPACITY:
            return SAMPLER2D_OPACITY_STAGE;
        case aiTextureType_SPECULAR:
            return SAMPLER2D_SPECULAR_STAGE;
        default:
        {
            return SAMPLER2D_TEXTURE0_STAGE;
        }
    }
}

void process_textures(const aiMaterial *material, blackboard::core::resources::Model &&model)
{
    aiString texturePath;
    auto &&images_map = model.meshes.back().images;
    std::cout << ">>>> " << material->GetName().C_Str() << std::endl;
    for (auto type = aiTextureType_NONE; type <= aiTextureType_UNKNOWN;
         type = static_cast<aiTextureType>(type + 1))
    {
        //If there are any textures of the given type in the material
        //        aiString baseColorFactor;
        //        if(AI_SUCCESS == material->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, &baseColorFactor))
        //        {
        //            std::cout << "AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR" << std::endl;
        //        }

        if (material->GetTextureCount(type) > 0)
        {
            //We only care about the first texture assigned we don't expect multiple to be assigned
            material->GetTexture(type, 0, &texturePath);
            std::cout << ">>>> " << Texture_type_names.at(get_texture_stage(type))
                      << " PATH: " << texturePath.C_Str() << std::endl;
            std::filesystem::path image_path = model.path() / texturePath.C_Str();
            auto image_path_string = image_path.string();
            std::replace(image_path_string.begin(), image_path_string.end(), '\\', '/');
            image_path = {image_path_string};
            assert(std::filesystem::exists(image_path));

            Image image;
            const auto success = image_load(std::move(image_path), std::forward<Image>(image));
            if (success)
            {
                images_map.insert({get_texture_stage(type), std::move(image)});
            }
        }
    }
}

void process_mesh(const aiMesh *ai_mesh, const aiScene *ai_scene,
                  blackboard::core::resources::Model &&model)
{
    model.meshes.push_back({});
    auto &&mesh = model.meshes.back();
    auto &vertices = mesh.vertices;
    auto &indices = mesh.indices;

    const auto total_vertices = ai_mesh->mNumVertices;
    vertices.reserve(total_vertices);

    std::span<aiVector3D> ai_vertices{ai_mesh->mVertices, total_vertices};
    std::span<aiVector3D> ai_normals{ai_mesh->mNormals, total_vertices};
    std::span<aiVector3D> ai_tangents{ai_mesh->mTangents, total_vertices};
    std::span<aiVector3D> ai_bitangents{ai_mesh->mBitangents, total_vertices};
    std::span<aiVector3D> ai_texcoords{ai_mesh->mTextureCoords[0], total_vertices};
    std::span<aiColor4D> ai_colors{ai_mesh->mColors[0], total_vertices};

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
    indices.reserve(ai_mesh->mNumFaces * 3);

    std::span<aiFace> ai_faces{ai_mesh->mFaces, ai_mesh->mNumFaces};
    for (auto &&face : ai_faces)
    {
        assert(face.mNumIndices == 3);
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }

    //Process material and texture info
    aiMaterial *material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
    process_textures(material, std::forward<Model>(model));
}

void process_node(const aiNode *ai_node, const aiScene *ai_scene, Model &&model)
{
    std::span<unsigned int> ai_node_mesh_indices{ai_node->mMeshes, ai_node->mNumMeshes};
    std::span<aiMesh *> ai_meshes{ai_scene->mMeshes, ai_scene->mNumMeshes};

    for (auto &&ai_mesh_index : ai_node_mesh_indices)
    {
        aiMesh *ai_mesh = ai_meshes[ai_mesh_index];
        process_mesh(ai_mesh, ai_scene, std::forward<blackboard::core::resources::Model>(model));
        model.meshes.back().transform = assimp_to_mat4(ai_node->mTransformation);
    }

    std::span<aiNode *> ai_children_nodes{ai_node->mChildren, ai_node->mNumChildren};

    for (auto &&ai_child_node : ai_children_nodes)
    {
        process_node(ai_child_node, ai_scene, std::forward<Model>(model));
    }
}

}    // namespace impl

static std::filesystem::path app_base_path{};

std::filesystem::path path()
{
#ifdef __APPLE__
    return app_base_path;
#else
    return app_base_path / "Resources";
#endif
}

void init(std::filesystem::path &&app_path)
{
    app_base_path = app_path;
    using namespace entt::literals;
    //    entt::meta<Texture_type>()
    //      .type("Texture_type"_hs)
    //      .data<Texture_type::Albedo>("Albedo"_hs)
    //      .prop(meta::property_name_key, std::string{"Albedo"})
    //      .data<Texture_type::Normal>("Normal"_hs)
    //      .prop(meta::property_name_key, std::string{"Normal"});
}

entt::resource_cache<Model> models_cache;

struct Model_loader : entt::resource_loader<Model_loader, Model>
{
    std::shared_ptr<Model> load(std::filesystem::path &&path) const
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

        auto model = std::make_shared<Model>(path);
        model->meshes.reserve(scene->mNumMeshes);
        impl::process_node(scene->mRootNode, scene, std::forward<Model>(*model.get()));

        return model;
    }
};

entt::id_type load_model(std::filesystem::path &&path)
{
    const auto key = entt::hashed_string{path.string().c_str()};
    if (models_cache.load<Model_loader>(key, std::forward<std::filesystem::path>(path)))
    {
        return key.value();
    }

    return entt::null;
}

Model &get_model_ref(const entt::id_type key)
{
    return models_cache.handle(key).get();
}

entt::resource_handle<Model> get_model_handle(const entt::id_type key)
{
    return models_cache.handle(key);
}

bool is_valid_model_key(const entt::id_type key)
{
    return key != entt::null && models_cache.contains(key);
}

}    // namespace blackboard::core::resources
