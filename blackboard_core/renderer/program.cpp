#include "program.h"

#include <app/resources.h>

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

constexpr auto shader_bin_extension = ".bin";

#ifdef __APPLE__
constexpr auto shader_platform_flags = " --platform osx";
constexpr auto shader_fragment_program_flags = " -p metal";
constexpr auto shader_vertex_program_flags = " -p metal";
constexpr auto shaderc_binary = "tools/osx/shaderc";
#elif _WIN32
constexpr auto shader_platform_flags = " --platform windows";
constexpr auto shader_fragment_program_flags = " -p ps_5_0";
constexpr auto shader_vertex_program_flags = " -p vs_5_0";
constexpr auto shaderc_binary = "tools/win/shaderc.exe";
#endif

bgfx::ShaderHandle loadShader(const std::filesystem::path &filePath)
{
    if (std::filesystem::exists(filePath))
    {
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);
        std::streampos fileSize = file.tellg();
        const bgfx::Memory *mem = bgfx::alloc(static_cast<uint32_t>(fileSize) + 1);
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char *>(mem->data), fileSize);
        file.close();
        auto handle = bgfx::createShader(mem);
        if (isValid(handle))
        {
            bgfx::setName(handle, filePath.string().c_str());
        }
        return handle;
    }
    return {bgfx::kInvalidHandle};
}

int compileShader(const std::filesystem::path &shaderFile, blackboard::core::renderer::Program::Type type)
{
    std::string cmd =
      blackboard::core::resources::path().append(shaderc_binary).string();    // shaderc binary
    cmd.append(" -f " + shaderFile.string());    // input file
    cmd.append(" -o " + shaderFile.string() + shader_bin_extension);    // output file
    cmd.append(" -i " +
               blackboard::core::resources::path().append("shaders/common").string());    // include path
    cmd.append(blackboard::core::renderer::Program::TypeFlag[type]);    // shader type
    cmd.append(shader_platform_flags);    // platform flags
    switch (type)
    {
        case blackboard::core::renderer::Program::VERTEX:
            cmd.append(shader_vertex_program_flags);    // platform flags
            break;
        case blackboard::core::renderer::Program::FRAGMENT:
            cmd.append(shader_fragment_program_flags);    // platform flags
            break;
        default:
            break;
    }
    return system(cmd.c_str());
}

}    // namespace

namespace blackboard::core::renderer {

const std::array<std::string, Program::Type::COUNT> Program::TypeFlag{" --type v ", " --type f "};

bool Program::init(const std::filesystem::path &vshPath, const std::filesystem::path &fshPath)
{
    const auto vertErrorCode = compileShader(vshPath, Type::VERTEX);
    const auto fragErrorCode = compileShader(fshPath, Type::FRAGMENT);
    if (vertErrorCode != 0 || fragErrorCode != 0)
    {
        return false;
    }
    const auto vsh = loadShader(vshPath.string() + shader_bin_extension);
    const auto fsh = loadShader(fshPath.string() + shader_bin_extension);
    if (!bgfx::isValid(vsh) && !bgfx::isValid(fsh))
    {
        return false;
    }
    if (bgfx::isValid(m_program))
        bgfx::destroy(m_program);
    m_vsh = vsh;
    m_fsh = fsh;
    m_program = bgfx::createProgram(m_vsh, m_fsh, true);
    return true;
}

const bgfx::ProgramHandle Program::program() const
{
    return m_program;
}

}    // namespace blackboard::core::renderer
