#pragma once
#include "action_b.h"

#include <entt/entt.hpp>

#include <iostream>

namespace b {
void execute(void *meta_action)
{
    auto action = static_cast<Action*>(meta_action);
    std::cout << entt::type_id<Action>().name() << " has value: " << action->value <<std::endl;
}
}
