#include <core/src/app.h>

#include <imgui/imgui.h>

class Basic_app : public blackboard::App
{
public:
    Basic_app(const char* title) : blackboard::App(title)
    {  
    }
    
    void update() override
    {
        ImGui::ShowUserGuide();
        ImGui::ShowDemoWindow();
    }
};

BLACKBOARD_APP(Basic_app, "Example App");
