#include "module.h"

#include "action_a.h"
#include "action_b.h"
#include "action_m.h"
#include "executor_a.h"
#include "executor_b.h"
#include "executor_m.h"

#include <chrono>
#include <thread>

#include <entt/entt.hpp>
#include <entt/meta/template.hpp>
#include <concurrentqueue.h>


template<typename A, void free_function(const A&)>
void register_action()
{
    using namespace entt::literals;
    const std::string type_name{entt::type_id<A>().name()};
    auto hashed_name = entt::hashed_string{type_name.c_str()};
    entt::meta<A>().type(hashed_name);

    std::string executor_str("execute_");
    executor_str.append(type_name);

    auto hashed_executor_str = entt::hashed_string{executor_str.c_str()};
    entt::meta<A>().template func<free_function>(hashed_executor_str);
}

Module::Module(moodycamel::ConcurrentQueue<entt::meta_any> &_queue):queue(_queue)
{
    register_action<a::Action, a::execute>();
    register_action<b::Action, b::execute>();
    register_action<m::Action, m::execute>();

    action_pusher = std::thread([&](){
        using namespace std::chrono_literals;
        while (push_actions) {
            queue.enqueue(a::Action{50});
            queue.enqueue(b::Action{20});
            queue.enqueue(m::Action{std::make_unique<int>(10)});
//            queue.enqueue(std::string("ouch!"));
            std::this_thread::sleep_for(500ms);
        }
    });
}

Module::~Module()
{
     push_actions = false;
     action_pusher.join();
}
