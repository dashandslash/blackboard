#include "app.h"

#include <functional>
#include <iostream>
#include <thread>

namespace blackboard {
template<>
App<Render_api::none>::App(const char *app_name, const uint16_t width, const uint16_t height)
{}

template<>
void App<Render_api::none>::run()
{
    while (running)
    {
        on_update();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(std::chrono::milliseconds(update_rate));
    }
}

template<>
App<Render_api::none>::~App()
{
    running = false;
}
}    // namespace blackboard
