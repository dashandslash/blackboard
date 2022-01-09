cmake_minimum_required(VERSION 3.21)
project(core)

set(SOURCES
    app/app_headless.cpp
    app/app_sdl_metal.cpp
    gui/gui.cpp
    renderer/platform/imgui_impl_sdl_bgfx.cpp
    renderer/renderer.cpp
    )

set(HEADERS 
    app/app.h
    gui/gui.h
    renderer/platform/imgui_impl_sdl_bgfx.h
    renderer/renderer.h
    )

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      bgfx
                      ImGui
                      SDL2-static
                      )

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

target_compile_definitions(${PROJECT_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS" "-DENTRY_CONFIG_IMPLEMENT_MAIN=1" )
# target_compile_definitions(${PROJECT_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS")


if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:\"mainCRTStartup\"" )
endif()

