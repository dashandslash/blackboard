
#include "imgui_impl_sdl_bgfx.h"
#include "imgui/imgui.h"

#include "bgfx/bgfx.h"
#include "bgfx/embedded_shader.h"
#include "bx/math.h"
#include "bx/timer.h"

#include "fs_ocornut_imgui.bin.h"
#include "vs_ocornut_imgui.bin.h"

#include <SDL.h>
#include <SDL_syswm.h>

#include <string>
#include <vector>

// Data
namespace blackboard {
namespace renderer {

static uint8_t main_view_id{255};
static bool is_init{false};
static bgfx::TextureHandle font_texture = BGFX_INVALID_HANDLE;
static bgfx::ProgramHandle shader_handle = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle uniform_texture = BGFX_INVALID_HANDLE;
static bgfx::VertexLayout vertex_layout;
static std::vector<bgfx::ViewId> free_view_ids;
static bgfx::ViewId sub_view_id = 100;

static bgfx::ViewId allocate_view_id() {
    if (!free_view_ids.empty()) {
        auto id = free_view_ids.back();
        free_view_ids.pop_back();
        return id;
    }
    return sub_view_id++;
}

static void free_view_id(bgfx::ViewId id) {
    free_view_ids.push_back(id);
}

static const bgfx::EmbeddedShader s_embeddedShaders[] = {
    BGFX_EMBEDDED_SHADER(vs_ocornut_imgui),
    BGFX_EMBEDDED_SHADER(fs_ocornut_imgui), BGFX_EMBEDDED_SHADER_END()};

bool checkAvailTransientBuffers(uint32_t _numVertices, const bgfx::VertexLayout& _layout, uint32_t _numIndices) {
    return _numVertices == bgfx::getAvailTransientVertexBuffer(_numVertices, _layout) && (0 == _numIndices || _numIndices == bgfx::getAvailTransientIndexBuffer(_numIndices));
}

enum class BgfxTextureFlags : uint32_t {
    Opaque = 1u << 31,
    PointSampler = 1u << 30,
    All = Opaque | PointSampler,
};

void* native_window_handle(void* window) {
    SDL_Window* sdl_window = (SDL_Window*)window;
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(sdl_window, &wmi)) {
        return NULL;
    }
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#if ENTRY_CONFIG_USE_WAYLAND
    wl_egl_window* win_impl = (wl_egl_window*)SDL_GetWindowData(sdl_window, "wl_egl_window");
    if (!win_impl) {
        int width, height;
        SDL_GetWindowSize(sdl_window, &width, &height);
        struct wl_surface* surface = wmi.info.wl.surface;
        if (!surface)
            return nullptr;
        win_impl = wl_egl_window_create(surface, width, height);
        SDL_SetWindowData(sdl_window, "wl_egl_window", win_impl);
    }
    return (void*)(uintptr_t)win_impl;
#else
    return (void*)wmi.info.x11.window;
#endif
#elif BX_PLATFORM_OSX
    return wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
    return wmi.info.win.window;
#endif // BX_PLATFORM_
}

struct imgui_viewport_data {
    bgfx::FrameBufferHandle frameBufferHandle;
    bgfx::ViewId viewId = 0;
    uint16_t width = 0;
    uint16_t height = 0;
};

static void ImguiBgfxOnCreateWindow(ImGuiViewport* viewport) {
    auto data = new imgui_viewport_data();
    viewport->RendererUserData = data;
    // Setup view id and size
    data->viewId = allocate_view_id();
    data->width = bx::max<uint16_t>((uint16_t)viewport->Size.x, 1);
    data->height = bx::max<uint16_t>((uint16_t)viewport->Size.y, 1);
    // Create frame buffer
    data->frameBufferHandle = bgfx::createFrameBuffer(native_window_handle((SDL_Window*)viewport->PlatformHandle), data->width, data->height);
    // Set frame buffer
    bgfx::setViewFrameBuffer(data->viewId, data->frameBufferHandle);
}

static void ImguiBgfxOnDestroyWindow(ImGuiViewport* viewport)
{
    if (auto data = (imgui_viewport_data*)viewport->RendererUserData; data)
    {
        viewport->RendererUserData = nullptr;
        free_view_id(data->viewId);
        bgfx::destroy(data->frameBufferHandle);
        data->frameBufferHandle.idx = bgfx::kInvalidHandle;
        delete data;
    }
}

static void ImguiBgfxOnSetWindowSize(ImGuiViewport* viewport, ImVec2 size) {
    ImguiBgfxOnDestroyWindow(viewport);
    ImguiBgfxOnCreateWindow(viewport);
}

static void ImguiBgfxOnRenderWindow(ImGuiViewport* viewport, void*)
{
    if(auto data = (imgui_viewport_data*)viewport->RendererUserData; data)
    {
        ImGui_Impl_sdl_bgfx_Render(data->viewId, viewport->DrawData, !(viewport->Flags & ImGuiViewportFlags_NoRendererClear) ? 0x000000ff : 0);
    }
}

void ImGui_Impl_sdl_bgfx_Render(bgfx::ViewId view_id, ImDrawData* draw_data, uint32_t clearColor) {
    if (clearColor) {
        bgfx::setViewClear(view_id, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, clearColor, 1.0f, 0);
    }
    if (ImGuiIO& io = ImGui::GetIO(); io.DisplaySize.x <= 0 || io.DisplaySize.y <= 0)
    {
        return;
    }

    bgfx::touch(view_id);

    const auto L = -draw_data->DisplayPos.x;
    const auto R = L + draw_data->DisplaySize.x;
    const auto T = -draw_data->DisplayPos.y;
    const auto B = T + draw_data->DisplaySize.y;
    float ortho[16];
    bx::mtxOrtho(ortho, L, R, T, B, 0.0f, 1000.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(view_id, nullptr, ortho);
    bgfx::setViewRect(view_id, 0, 0, (uint16_t)draw_data->DisplaySize.x, (uint16_t)draw_data->DisplaySize.y);

    // Render command lists
    for (int32_t ii = 0, num = draw_data->CmdListsCount; ii < num; ++ii) {
        const ImDrawList* draw_list = draw_data->CmdLists[ii];
        uint32_t num_vertices = (uint32_t)draw_list->VtxBuffer.size();
        uint32_t num_indices = (uint32_t)draw_list->IdxBuffer.size();

        if (num_vertices == 0 || num_indices == 0) {
            continue;
        }

        if (!checkAvailTransientBuffers(num_vertices, vertex_layout, 0)) {
            // not enough space in transient buffer just quit drawing the rest...
            break;
        }
        bgfx::TransientVertexBuffer tvb;
        bgfx::allocTransientVertexBuffer(&tvb, num_vertices, vertex_layout);

        ImDrawVert* verts = (ImDrawVert*)tvb.data;
        for (uint32_t v = 0; v < num_vertices; v++)
        {
            verts[v].uv = draw_list->VtxBuffer.begin()[v].uv;
            verts[v].col = draw_list->VtxBuffer.begin()[v].col;
            verts[v].pos.x = draw_list->VtxBuffer.begin()[v].pos.x - draw_data->DisplayPos.x;
            verts[v].pos.y = draw_list->VtxBuffer.begin()[v].pos.y - draw_data->DisplayPos.y;
        }

        auto index_buffer_handle = bgfx::createIndexBuffer(bgfx::copy(draw_list->IdxBuffer.begin(), num_indices * sizeof(ImDrawIdx)));
        uint32_t offset = 0;
        for (const ImDrawCmd& cmd : draw_list->CmdBuffer)
        {
            if (cmd.UserCallback) {
                cmd.UserCallback(draw_list, &cmd);
            }
            else if (0 != cmd.ElemCount) {
                uint64_t state = 0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;
                uint32_t sampler_state = 0;

                bgfx::TextureHandle th = font_texture;
                bgfx::ProgramHandle program = shader_handle;

                auto alphaBlend = true;
                if (cmd.TextureId != nullptr) {
                    auto textureInfo = (uintptr_t)cmd.TextureId;
                    if (textureInfo & (uint32_t)BgfxTextureFlags::Opaque) {
                        alphaBlend = false;
                    }
                    if (textureInfo & (uint32_t)BgfxTextureFlags::PointSampler) {
                        sampler_state = BGFX_SAMPLER_POINT;
                    }
                    textureInfo &= ~(uint32_t)BgfxTextureFlags::All;
                    th = { (uint16_t)textureInfo };
                }
                if (alphaBlend) {
                    state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
                }
                const auto position = draw_data->DisplayPos;
                const uint16_t xx0(bx::max(cmd.ClipRect.x, 0.0f) - position.x);
                const uint16_t yy0(bx::max(cmd.ClipRect.y, 0.0f) - position.y);
                const uint16_t xx1(bx::max(cmd.ClipRect.z, 0.0f) - position.x);
                const uint16_t yy1(bx::max(cmd.ClipRect.w, 0.0f) - position.y);
                bgfx::setScissor(xx0, yy0, xx1, yy1);

                bgfx::setState(state);
                float mtx[16];
                bx::mtxTranslate(mtx, -draw_data->DisplayPos.x, -draw_data->DisplayPos.y, 0.0);
                bgfx::setTransform(mtx);
                bgfx::setTexture(0, uniform_texture, th, sampler_state);
                bgfx::setVertexBuffer(0, &tvb, 0, num_vertices);
                bgfx::setIndexBuffer(index_buffer_handle, offset, cmd.ElemCount);
                bgfx::submit(view_id, program);
            }

            offset += cmd.ElemCount;
        }

        bgfx::destroy(index_buffer_handle);
    }
}

void ImGui_Implbgfx_CreateDeviceObjects()
{
    const auto type = bgfx::getRendererType();
    shader_handle = bgfx::createProgram(
        bgfx::createEmbeddedShader(s_embeddedShaders, type, "vs_ocornut_imgui"),
        bgfx::createEmbeddedShader(s_embeddedShaders, type, "fs_ocornut_imgui"),
        true);

    vertex_layout.begin()
        .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

    uniform_texture = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);

    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    // Upload texture to graphics system
    font_texture = bgfx::createTexture2D(
        (uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::BGRA8,
        0, bgfx::copy(pixels, width * height * 4));
    is_init = bgfx::isValid(font_texture);
    // Store our identifier
    io.Fonts->TexID = (void*)(intptr_t)font_texture.idx;
}

void ImGui_Implbgfx_InvalidateDeviceObjects()
{
    if (bgfx::isValid(shader_handle))
    {
        bgfx::destroy(shader_handle);
        shader_handle.idx = bgfx::kInvalidHandle;
    }

    if (isValid(font_texture)) {
        bgfx::destroy(font_texture);
        ImGui::GetIO().Fonts->TexID = 0;
        font_texture.idx = bgfx::kInvalidHandle;
    }
}

void ImGui_Impl_sdl_bgfx_Init(int view)
{
    main_view_id = (uint8_t)(view & 0xff);
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Renderer_CreateWindow = ImguiBgfxOnCreateWindow;
    platform_io.Renderer_DestroyWindow = ImguiBgfxOnDestroyWindow;
    platform_io.Renderer_SetWindowSize = ImguiBgfxOnSetWindowSize;
    platform_io.Renderer_RenderWindow = ImguiBgfxOnRenderWindow;
}

void ImGui_Impl_sdl_bgfx_Shutdown()
{
    ImGui_Implbgfx_InvalidateDeviceObjects();
}

void ImGui_Impl_sdl_bgfx_NewFrame()
{
    if (!is_init) {
        ImGui_Implbgfx_CreateDeviceObjects();
    }
}
}
}
