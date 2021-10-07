#pragma once

#include <chrono>
#include <thread>

#include <entt/entt.hpp>
#include <entt/meta/template.hpp>
#include <concurrentqueue.h>

struct Module
{
Module() = delete;
Module(moodycamel::ConcurrentQueue<entt::meta_any> &_queue);
~Module();

moodycamel::ConcurrentQueue<entt::meta_any> &queue;
std::thread action_pusher;
bool push_actions{true};
};
