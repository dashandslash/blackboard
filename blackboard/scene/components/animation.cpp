#include "animation.h"

#include "transform.h"

#include <glm/glm.hpp>

namespace blackboard {

namespace components {

template<>
void Animation<Transform>::update()
{
    m_current_value.scale = glm::mix(start_value().scale, end_value().scale, current_time());
    m_current_value.translation =
      glm::mix(start_value().translation, end_value().translation, current_time());
    m_current_value.rotation = glm::slerp(start_value().rotation, end_value().rotation, current_time());
}

}    // namespace components
}    // namespace blackboard
