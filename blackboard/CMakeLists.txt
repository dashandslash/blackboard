cmake_minimum_required(VERSION 3.21)

project(blackboard_lib)

set(SOURCES
    app/app.cpp
    gui/components.cpp
    gui/gui.cpp
    platform/window.cpp
    meta/meta.cpp
    renderer/camera.cpp
    renderer/platform/imgui_impl_sdl_bgfx.cpp
    renderer/program.cpp
    renderer/renderer.cpp
    scene/components/animation.cpp
    state/state.cpp
)

set(HEADERS 
    app/app.h
    app/resources.h
    gui/components.h
    gui/gui.h
    platform/window.h
    meta/meta.h
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
                      SDL2-static
                      bgfx
                      bx
                      EnTT
			          ImGui
                      glm
)

target_include_directories(${PROJECT_NAME}
                           PUBLIC
                           ${PROJECT_SOURCE_DIR}
                           ${FETCHCONTENT_BASE_DIR}
                           ${imgui_SOURCE_SYMLINK_DIR}
                           ${bgfx_cmake_SOURCE_DIR}/bx/inlcude
                        #    ${SDL2_INCLUDE_DIRS}
                        #    ${SDL2_IMAGE_INCLUDE_DIRS}
                        #    ${SDL2_TTF_INCLUDE_DIRS}
                        #    ${SDL2_MIXER_INCLUDE_DIRS}
)

if(WIN32)
target_include_directories(${PROJECT_NAME}
                           PUBLIC
                           ${bgfx_cmake_SOURCE_DIR}/bx/include/compat/msvc
)
endif()

target_compile_definitions(${PROJECT_NAME}
                           PRIVATE
                           "-D_CRT_SECURE_NO_WARNINGS"
                           "-D__STDC_FORMAT_MACROS"
)

target_compile_definitions(${PROJECT_NAME} PUBLIC "BX_CONFIG_DEBUG=$<CONFIG:Debug>")
target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_DEFINE_MATH_OPERATORS)
