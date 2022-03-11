#pragma once
#include "program.h"

#include <app/resources.h>
#include <utils/watchdog.h>

#include <bgfx/bgfx.h>
#include <entt/core/type_info.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include <array>
#include <filesystem>
#include <unordered_map>
#include <variant>

template<class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace blackboard::core::renderer {
namespace material {

struct Uniform
{
    std::array<float, 3> u_camera_position{0.0f, 0.0f, 0.0f};
    float u_time{0.0f};
    std::array<float, 4> u_color{1.0f, 0.0f, 0.0f, 1.0f};
    std::array<float, 4> u_edge_color{0.0f, 1.0f, 0.0f, 1.0f};
    float u_edge_thickness{1.0f};
    std::array<float, 3> padding{0.0f, 0.0f, 0.0f};
};

struct Material
{
    struct Material_concept
    {
        virtual ~Material_concept() = default;
        virtual std::unique_ptr<Material_concept> clone() const = 0;
    };

    template<typename T>
    struct Material_model : public Material_concept
    {
        virtual std::unique_ptr<Material_concept> clone() const override
        {
            return std::make_unique<Material_concept>(*this);
        }
    };

    Material(const std::filesystem::path &vert_shader, const std::filesystem::path &frag_shader)
        : m_vert_path(vert_shader), m_frag_path(frag_shader)
    {
        init();
    }

    ~Material()
    {}

    void init()
    {
        shader_prog.init(m_vert_path, m_frag_path);
    }

    const bgfx::ProgramHandle program_handle() const
    {
        return shader_prog.program();
    }

    const bool isValid() const
    {
        return bgfx::isValid(shader_prog.program());
    }

    std::tuple<std::filesystem::path, std::filesystem::path> paths() const
    {
        return {m_vert_path, m_frag_path};
    }

    Program shader_prog;
    std::filesystem::path m_vert_path;
    std::filesystem::path m_frag_path;
    std::mutex mutex;

private:
    std::unique_ptr<Material_concept> p_impl;
};

struct UniformColor : public Material
{
    inline static std::string vert_path{"shaders/uniform_color/uniform_color.vert"};
    inline static std::string frag_path{"shaders/uniform_color/uniform_color.frag"};
    UniformColor()
        : Material(blackboard::core::resources::path() / "shaders/uniform_color/uniform_color.vert",
                   blackboard::core::resources::path() / "shaders/uniform_color/uniform_color.frag")
    {}

    ~UniformColor() = default;
};

using MaterialType = std::shared_ptr<Material>;
inline std::unordered_map<entt::id_type, MaterialType> materials;

struct Manager
{
    Manager()
    {
        materials.insert({entt::type_id<UniformColor>().hash(), std::make_shared<UniformColor>()});

        for (auto &entry : materials)
        {
            const auto &[vert_shader, frag_shader] = entry.second->paths();
            const auto callback_multi = [&entry](const std::vector<std::filesystem::path> &path) {
                entry.second->init();
            };
            const auto callback = [&entry](const std::filesystem::path &path) { entry.second->init(); };
            utils::wd::watch(vert_shader, callback);
            utils::wd::watch(frag_shader, callback);
            utils::wd::watchMany(vert_shader, callback_multi);
            utils::wd::watchMany(frag_shader, callback_multi);
        }
    }

    ~Manager()
    {
        shutdown();
    }

    inline void shutdown()
    {
        for (const auto &[name, material] : materials)
        {
            const auto &[vert_shader, frag_shader] = material->paths();
            utils::wd::unwatch(vert_shader);
            utils::wd::unwatch(frag_shader);
        }
        utils::wd::unwatchAll();

        materials.clear();
    }

    template<typename T>
    inline decltype(auto) material() const
    {
        assert(materials.contains(entt::type_id<T>().hash()));
        return materials.at(entt::type_id<T>().hash());
    }

    void set_uniform(bgfx::Encoder *encoder, Uniform *uniform)
    {
        if (encoder == NULL)
        {
            return;
        }

        encoder->setUniform(m_uniform_handle, uniform);
    }

    void set_uniform(Uniform *uniform)
    {
        static const uint16_t num{sizeof(Uniform) / sizeof(float[4])};
        if (!bgfx::isValid(m_uniform_handle))
        {
            m_uniform_handle = bgfx::createUniform("u_uniform", bgfx::UniformType::Vec4, num);
        }
        bgfx::setUniform(m_uniform_handle, uniform, num);
    }

private:
    bgfx::UniformHandle m_uniform_handle = BGFX_INVALID_HANDLE;
};
}    // namespace material

}    // namespace blackboard::core::renderer
