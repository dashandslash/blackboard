cmake_minimum_required(VERSION 3.21)
project(core)

set(SOURCES
    app/app.cpp
    gui/gui.cpp
    platform/window.cpp
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
    platform/window.h
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
                      PUBLIC
                      SDL2-static
                      bgfx
                      EnTT
                      ImGui
                      glm
                      range-v3::range-v3
                      )
target_include_directories(${PROJECT_NAME}
                           PUBLIC
                           ${PROJECT_SOURCE_DIR}
                           ${SDL2_INCLUDE_DIRS}
                           ${SDL2_IMAGE_INCLUDE_DIRS}
                           ${SDL2_TTF_INCLUDE_DIRS}
                           ${SDL2_MIXER_INCLUDE_DIRS}
                           ${PROJECT_SOURCE_DIR}/external/
                           ${PROJECT_SOURCE_DIR}/external/bgfx.cmake/bgfx/include
                           ${PROJECT_SOURCE_DIR}/external/bgfx.cmake/bx/include
                           ${PROJECT_SOURCE_DIR}/external/imgui
)

if(WIN32)
#    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:\"mainCRTStartup\"" )
    target_include_directories(${PROJECT_NAME}
                               PUBLIC
                               ${PROJECT_SOURCE_DIR}/external/bgfx.cmake/bx/include/compat/msvc
)
endif()

target_compile_definitions(${PROJECT_NAME}
                           PRIVATE
                           "-D_CRT_SECURE_NO_WARNINGS"
                           "-D__STDC_FORMAT_MACROS"
)
# target_compile_definitions(${PROJECT_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS")

add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)

add_subdirectory(external)
