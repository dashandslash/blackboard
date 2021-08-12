cmake_minimum_required(VERSION 3.21)
project(third-party)

# <bgfx
set( BGFX_BUILD_TOOLS  ON  CACHE INTERNAL "" )
set( BGFX_BUILD_EXAMPLES  ON CACHE INTERNAL "" )
set( BGFX_CUSTOM_TARGETS  OFF CACHE INTERNAL "" )
add_subdirectory( bgfx.cmake )
# bgfx>

# <ImGui
set(imgui_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(imgui_SOURCES
  ${imgui_DIR}/imgui.cpp
  ${imgui_DIR}/imgui_demo.cpp
  ${imgui_DIR}/imgui_draw.cpp
  ${imgui_DIR}/imgui_tables.cpp
  ${imgui_DIR}/imgui_widgets.cpp
  ${imgui_DIR}/misc/cpp/imgui_stdlib.cpp
  ${imgui_DIR}/backends/imgui_impl_sdl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ImGuiColorTextEdit/TextEditor.cpp
  )
set(imgui_HEADERS
  ${imgui_DIR}/imconfig.h
  ${imgui_DIR}/imgui.h
  ${imgui_DIR}/imstb_rectpack.h
  ${imgui_DIR}/imstb_truetype.h
  ${imgui_DIR}/imgui_internal.h
  ${imgui_DIR}/imstb_textedit.h
  ${imgui_DIR}/backends/imgui_impl_sdl.h
  ${imgui_DIR}/misc/cpp/imgui_stdlib.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ImGuiColorTextEdit/TextEditor.h
  )
add_library(ImGui STATIC ${imgui_SOURCES} ${imgui_HEADERS})
target_include_directories(ImGui PUBLIC
  $<BUILD_INTERFACE:${imgui_DIR}>
# ImGui>

#  set(fmt_DIR ${CMAKE_CURRENT_SOURCE_DIR}/fmt/include)
)

# <SDL
add_subdirectory(SDL)
include_directories(SDL/include)
# SDL>
