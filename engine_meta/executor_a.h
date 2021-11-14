#pragma once
#include "action_a.h"

#include <entt/entt.hpp>

#include <iostream>

namespace a {
void execute(const Action &action)
{
    std::cout << entt::type_id<Action>().name() << " has value: " << action.value <<std::endl;
}
}
