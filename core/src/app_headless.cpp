#include "app.h"

#include <thread>

namespace blackboard
{
template<> App<Render_api::none>::App(const char* app_name)
{

}

template<> void App<Render_api::none>::run()
{
    while (running)
    {
        // call to virtual update function
        update();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(std::chrono::milliseconds(update_rate));
    }
}
    
template<> App<Render_api::none>::~App()
{
    running = false;
}
}
