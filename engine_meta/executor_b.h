#pragma once
#include "action_b.h"

#include <entt/entt.hpp>

#include <iostream>

namespace b {
void execute(const Action &action)
{
    std::cout << entt::type_id<Action>().name() << " has value: " << action.value <<std::endl;
}
}
