#include "app.h"

#include <functional>
#include <iostream>
#include <thread>

namespace blackboard {
template<>
App_headless::App(const char *app_name, const uint16_t width, const uint16_t height)
{}

template<>
void App_headless::run()
{
    while (m_running)
    {
        on_update();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(std::chrono::milliseconds(m_update_rate));
    }
}

template<>
App_headless::~App()
{
    m_running = false;
}
}    // namespace blackboard
