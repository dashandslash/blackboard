#pragma once
#include <bgfx/bgfx.h>

#include <filesystem>
#include <string>

namespace blackboard::renderer {

struct Program
{
    enum Type : uint8_t
    {
        VERTEX = 0,
        FRAGMENT,
        COUNT
    };

    Program(): m_vsh(BGFX_INVALID_HANDLE), m_fsh(BGFX_INVALID_HANDLE), m_program(BGFX_INVALID_HANDLE)
    {}

    ~Program()
    {
        if (bgfx::isValid(m_program))
            bgfx::destroy(m_program);
        if (bgfx::isValid(m_vsh))
            bgfx::destroy(m_vsh);
        if (bgfx::isValid(m_fsh))
            bgfx::destroy(m_fsh);
    }

    bool init(const std::filesystem::path &vshPath, const std::filesystem::path &fshPath);

    const bgfx::ProgramHandle program() const;

    static const std::array<std::string, Type::COUNT> TypeFlag;

    bgfx::ShaderHandle m_vsh;
    bgfx::ShaderHandle m_fsh;
    bgfx::ProgramHandle m_program;
};
}    // namespace blackboard::renderer
