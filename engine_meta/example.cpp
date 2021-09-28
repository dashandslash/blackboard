#include <core/src/app.h>

#include <imgui/imgui.h>
#include <entt/entt.hpp>
#include <entt/meta/template.hpp>
#include <concurrentqueue.h>

#include <chrono>
#include <iostream>
#include <thread>


namespace a {
struct Action
{
    int value{0};
    Action& get()
    {
        return *this;
    }
};

void execute(Action &a)
{
    std::cout << entt::type_id<Action>().name() << " has value: " << a.value <<std::endl;
}
}

namespace b {
struct Action
{
    int value{0};
    Action& get()
    {
        return *this;
    }
};

void execute(Action &a)
{
    std::cout << entt::type_id<Action>().name() << " has value: " << a.value <<std::endl;
}
}

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

moodycamel::ConcurrentQueue<entt::meta_any> queue;

void update()
{
    entt::meta_any any_item;
    while(queue.try_dequeue(any_item))
    {
        using namespace entt::literals;
        any_item.invoke("execute"_hs, any_item.invoke("get"_hs).as_ref());
    }

}

int main( int argc, char* argv[] )
{
    register_action<a::Action, a::execute>();
    register_action<b::Action, b::execute>();
    
    bool push_actions{true};
    
    auto action_pusher = std::thread([&](){
        using namespace std::chrono_literals;
        while (push_actions) {
            queue.enqueue(a::Action{50});
            queue.enqueue(b::Action{20});
            std::this_thread::sleep_for(500ms);
        }
    });
    
    blackboard::App<blackboard::Render_api::none> app("engine-meta");
    app.update = update;
    app.run();
   
    push_actions = false;
    action_pusher.join();

    return 0;
}
