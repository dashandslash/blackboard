#pragma once
#include "action_b.h"

#include <entt/entt.hpp>

#include <iostream>

namespace b {

void execute(Action &a)
{
    std::cout << entt::type_id<Action>().name() << " has value: " << a.value <<std::endl;
}
}
