#include "animation.h"

#include "transform.h"

#include <glm/glm.hpp>

namespace blackboard::core {

namespace components {

template<>
Transform Animation::update(const Transform &from, const Transform &to, const float interp)
{
    Transform tr;
    tr.rotation = glm::slerp(from.rotation, to.rotation, interp);
    tr.scale = glm::mix(from.scale, to.scale, interp);
    tr.translation = glm::mix(from.translation, to.translation, interp);
    return tr;
}

}    // namespace components
}    // namespace blackboard::core
