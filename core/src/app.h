#pragma once
#include "gui.h"

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
    virtual void update() = 0;
    void run();
protected:
    Window<SDL_Window> m_window;
    bool running{true};
    uint32_t update_rate{16};
};

//#define BLACKBOARD_HEADLESS

#ifndef BLACKBOARD_HEADLESS
#ifdef __APPLE__
using BaseApp = blackboard::App<blackboard::Render_api::metal>;
#endif
#else
using BaseApp = blackboard::App<blackboard::Render_api::none>;
#endif

#define BLACKBOARD_APP(APP, APP_NAME) \
int main( int argc, char* argv[] ) \
{ \
    APP app(APP_NAME); \
    app.run(); \
    return 0; \
}
}
