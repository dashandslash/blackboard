#pragma once
#include "gui.h"

#include <string>

namespace blackboard
{
class App
{
public:
    App() = delete;
    explicit App(const char* app_name);
    ~App();
    virtual void update() = 0;
    void run();
private:
    gui::Window_data m_window;
};

#define BLACKBOARD_APP(APP, APP_NAME) \
int main( int argc, char* argv[] ) \
{ \
    APP app(APP_NAME); \
    app.run(); \
    return 0; \
}

}
