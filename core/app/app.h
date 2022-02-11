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
    explicit App(const char *app_name, const uint16_t width = 640u * 2,
                 const uint16_t height = 360u * 2, const bool fullscreen = false);
    ~App();
    void run();
    std::function<void()> on_init{[]() { std::cout << "init function not defined" << std::endl; }};
    std::function<void()> on_update{[]() { std::cout << "update function not defined" << std::endl; }};
    std::function<void(const uint16_t, const uint16_t)> on_resize{
      [](const uint16_t width, const uint16_t height) {
          std::cout << "window resize function not defined" << std::endl;
      }};
    virtual std::filesystem::path resources_path() { return {"/"}; }

    static float delta_time()
    {
        using namespace std::chrono;
        return duration_cast<duration<float, std::milli>>(steady_clock::now() - m_prev_time).count();
    }

    static float elapsed_time()
    {
        return std::chrono::duration<float>(std::chrono::steady_clock::now() - m_start_time).count();
    }

    // protected:
    gui::Window<WindowType> m_window;
    bool m_running{true};
    uint32_t m_update_rate{16};
    static const decltype(RendererApi) m_renderer_api;
    inline static std::chrono::time_point<std::chrono::steady_clock> m_start_time =
      std::chrono::steady_clock::now();
    inline static std::chrono::time_point<std::chrono::steady_clock> m_prev_time =
      std::chrono::steady_clock::now();
};

#ifdef __EMSCRIPTEN__
using App_sdl_emscripten = App<renderer::Api::WebGL, SDL_Window>;
#endif
using App_sdl_metal = App<renderer::Api::metal, SDL_Window>;
using App_headless = App<renderer::Api::none, void>;
}    // namespace blackboard
