#include "animation.h"

#include "transform.h"

#include <glm/gtx/matrix_interpolation.hpp>

namespace blackboard {

namespace components {

template<>
void Animation<Transform>::update()
{
    m_current_value.set_transform(
      glm::interpolate(start_value().get_transform(), end_value().get_transform(), current_time()));
}

}    // namespace components
}    // namespace blackboard
