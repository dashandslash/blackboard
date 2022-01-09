#pragma once

#include "gui/gui.h"
#include "renderer/renderer.h"

#include <filesystem>
#include <iostream>
#include <string>

namespace blackboard {

template<renderer::Api RendererApi, typename WindowType>
class App
{
public:
    App() = delete;
    explicit App(const char *app_name, const uint16_t width = 1280u, const uint16_t height = 720u);
    ~App();
    void run();
    std::function<void()> on_update{[]() { std::cout << "update function not defined" << std::endl; }};
    virtual std::filesystem::path get_app_path() { return {"/"}; }

protected:
    gui::Window<WindowType> m_window;
    bool m_running{true};
    uint32_t m_update_rate{16};
    static const decltype(RendererApi) m_renderer_api;
};

using App_sdl_metal = App<renderer::Api::metal, SDL_Window>;
using App_headless = App<renderer::Api::none, void>;
}    // namespace blackboard
