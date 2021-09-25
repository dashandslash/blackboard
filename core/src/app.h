#pragma once
#include "gui.h"

#include <iostream>
#include <string>

namespace blackboard
{
template<Render_api T>
class App
{
public:
    App() = delete;
    explicit App(const char* app_name);
    ~App();
    void run();
    std::function<void()> update{[](){ std::cout << "update function not defined" << std::endl; }};
protected:
    Window<SDL_Window> m_window;
    bool running{true};
    uint32_t update_rate{16};
};
}
