#pragma once
#include "action_a.h"

#include <entt/entt.hpp>

#include <iostream>

namespace a {

void execute(Action &a)
{
    std::cout << entt::type_id<Action>().name() << " has value: " << a.value <<std::endl;
}
}
