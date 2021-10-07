
#include "module.h"
#include "action_a.h"
#include "action_b.h"
#include "executor_a.h"
#include "executor_b.h"

#include <chrono>
#include <thread>

#include <entt/entt.hpp>
#include <entt/meta/template.hpp>
#include <concurrentqueue.h>

template<typename A, void free_function(A&)>
void register_action()
{
    using namespace entt::literals;
    auto hashed_name = entt::hashed_string{std::string(entt::type_id<A>().name()).c_str()};
    entt::meta<A>().type(hashed_name);
    entt::meta<A>().template data<&A::value>("value"_hs);
    entt::meta<A>().template func<&A::get>("get"_hs);
    entt::meta<A>().template func<free_function>("execute"_hs);
}

Module::Module(moodycamel::ConcurrentQueue<entt::meta_any> &_queue):queue(_queue)
{
    register_action<a::Action, a::execute>();
    register_action<b::Action, b::execute>();
    
    action_pusher = std::thread([&](){
        using namespace std::chrono_literals;
        while (push_actions) {
            queue.enqueue(a::Action{50});
            queue.enqueue(b::Action{20});
            std::this_thread::sleep_for(500ms);
        }
    });
}

Module::~Module()
{
     push_actions = false;
     action_pusher.join();
}
