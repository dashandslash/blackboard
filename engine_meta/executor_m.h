#pragma once
#include "action_m.h"

#include <entt/entt.hpp>

#include <iostream>

namespace m {
void execute(const Action &action)
{
    std::cout << entt::type_id<Action>().name() << " has value: " << action.value.get() <<std::endl;
}
}
