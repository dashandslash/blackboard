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
        std::string executor_str("execute_");
        executor_str.append(any_item.type().info().name());
        auto hashed_executor_str = entt::hashed_string{executor_str.c_str()};
        
        const auto meta_action = entt::resolve(any_item.type().info());
        if (!meta_action)
        {
            std::cout << "Unkown action type: " << any_item.type().info().name() << std::endl;
            return;
        }

        const auto meta_executor = meta_action.func(hashed_executor_str);
        if (!meta_executor)
        {
            std::cout << "Unkown function type: " << hashed_executor_str.data() << std::endl;
            return;
        }

        meta_executor.invoke(hashed_executor_str, any_item.data());
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
