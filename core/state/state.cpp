#include "state.h"

namespace blackboard {

entt::entity State::create_entity()
{
    return m_registry.create();
}

}    // namespace blackboard
