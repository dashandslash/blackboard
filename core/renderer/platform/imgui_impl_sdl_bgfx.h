
#include <bgfx/bgfx.h>
#include <imgui/imgui.h>

struct SDL_Window;

namespace blackboard {
namespace renderer {
void ImGui_Impl_sdl_bgfx_Init(int view);
void ImGui_Impl_sdl_bgfx_Shutdown();
void ImGui_Impl_sdl_bgfx_NewFrame();
void ImGui_Impl_sdl_bgfx_Resize(SDL_Window *);
void ImGui_Impl_sdl_bgfx_Render(const bgfx::ViewId viewId, struct ImDrawData *draw_data,
                                uint32_t clearColor);

void ImGui_Implbgfx_RenderDrawLists(const uint16_t g_View, ImDrawData *draw_data);

// Use if you want to reset your rendering device without losing ImGui state.
void ImGui_Impl_sdl_bgfx_InvalidateDeviceObjects();
bool ImGui_Impl_sdl_bgfx_CreateDeviceObjects();

void *native_window_handle(void *window);
}    // namespace renderer
}    // namespace blackboard
