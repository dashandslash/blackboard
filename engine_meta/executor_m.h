#pragma once
#include "action_m.h"

#include <entt/entt.hpp>

#include <iostream>

namespace m {
void execute(void *meta_action)
{
    auto action = (Action*)meta_action;
    
    std::unique_ptr<int> value = std::move(action->value);
    std::cout << entt::type_id<Action>().name() << " has value: " << *value <<std::endl;
}
}
