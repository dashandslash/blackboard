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
    explicit App(const char *app_name, const uint16_t width = 1280u, const uint16_t height = 720u,
                 const bool fullscreen = false);
    ~App();
    void run();
    std::function<void()> on_update{[]() { std::cout << "update function not defined" << std::endl; }};
    std::function<void(const uint16_t, const uint16_t)> on_resize{
      [](const uint16_t width, const uint16_t height) {
          std::cout << "window resize function not defined" << std::endl;
      }};
    virtual std::filesystem::path get_app_path() { return {"/"}; }

    static float elapsed_time()
    {
        return std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - m_start_time)
          .count();
    }

protected:
    gui::Window<WindowType> m_window;
    bool m_running{true};
    uint32_t m_update_rate{16};
    static const decltype(RendererApi) m_renderer_api;
    inline static std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time =
      std::chrono::high_resolution_clock::now();
};

using App_sdl_metal = App<renderer::Api::metal, SDL_Window>;
using App_headless = App<renderer::Api::none, void>;
}    // namespace blackboard
