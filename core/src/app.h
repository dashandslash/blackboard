#pragma once
#include "gui.h"

#include <filesystem>
#include <iostream>
#include <string>

namespace blackboard
{
template<Render_api T>
class App
{
public:
    App() = delete;
    explicit App(const char* app_name, const uint16_t width = 1280u, const uint16_t height = 720u);
    ~App();
    void run();
    std::function<void()> on_update{[](){ std::cout << "update function not defined" << std::endl; }};
    virtual std::filesystem::path get_app_path(){return {"/"};}
protected:
    Window<SDL_Window> m_window;
    bool running{true};
    uint32_t update_rate{16};
};
}
