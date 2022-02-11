cmake_minimum_required(VERSION 3.21)
project(core)

set(SOURCES
    app/app.cpp
    gui/gui.cpp
    renderer/camera.cpp
    renderer/platform/imgui_impl_sdl_bgfx.cpp
    renderer/program.cpp
    renderer/renderer.cpp
    state/state.cpp
    )

set(HEADERS 
    app/app.h
    app/resources.h
    gui/gui.h
    renderer/platform/imgui_impl_sdl_bgfx.h
    renderer/camera.h
    renderer/material.h
    renderer/layouts.h
    renderer/program.h
    renderer/renderer.h
    renderer/utils.h
    scene/components/animation.h
    scene/components/transform.h
    state/state.h
    utils/watchdog.h
    )

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      bgfx
                      EnTT
                      ImGui
                      glm
                      range-v3::range-v3
                      SDL2-static
                      )

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

target_compile_definitions(${PROJECT_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS" "-DENTRY_CONFIG_IMPLEMENT_MAIN=1" )
# target_compile_definitions(${PROJECT_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS")


if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:\"mainCRTStartup\"" )
endif()

add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)
