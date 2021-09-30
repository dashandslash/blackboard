#pragma once
#include "action_a.h"

#include <entt/entt.hpp>

#include <iostream>

namespace a {
void execute(void *meta_action)
{
    auto action = (Action*)meta_action;
    std::cout << entt::type_id<Action>().name() << " has value: " << action->value <<std::endl;
}
}
