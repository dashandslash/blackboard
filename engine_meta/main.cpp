#include "module.h"
#include <core/src/app.h>

#include <concurrentqueue.h>
#include <entt/entt.hpp>


moodycamel::ConcurrentQueue<entt::meta_any> queue;

void update()
{
    entt::meta_any any_item;

    while(queue.try_dequeue(any_item))
    {
        auto range = entt::resolve();

        using namespace entt::literals;
        std::string executor_str("execute_");
        executor_str.append(any_item.type().info().name());
        auto hashed_executor_str = entt::hashed_string{executor_str.c_str()};
        any_item.invoke(hashed_executor_str, any_item.data());
    }
}

int main( int argc, char* argv[] )
{
    Module module(queue);
    
    blackboard::App<blackboard::Render_api::none> app("engine-meta");
    app.update = update;
    app.run();

    return 0;
}
