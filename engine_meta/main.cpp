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
        using namespace entt::literals;
        any_item.invoke("execute"_hs, any_item.invoke("get"_hs).as_ref());
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
