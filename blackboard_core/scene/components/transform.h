#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

struct SDL_Window;

namespace blackboard::core::components {

struct Transform
{
    glm::vec3 translation{0.0f, 0.0f, 0.0f};
    glm::quat rotation;
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
    glm::vec3 skew{0.0f, 0.0f, 0.0f};
    glm::vec4 perspective;

    Transform() = default;
    Transform(const Transform &other) = default;
    Transform(const glm::vec3 &t): translation(t)
    {}

    glm::mat4 get_transform() const
    {
        return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) *
               glm::scale(glm::mat4(1.0f), scale);
    }

    void set_transform(const glm::mat4 &transform)
    {
        glm::decompose(transform, scale, rotation, translation, skew, perspective);
    }
};

}    // namespace blackboard::core::components
