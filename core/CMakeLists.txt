cmake_minimum_required(VERSION 3.21)
project(core)

set(SOURCES
    src/app_headless.cpp
    src/app_sdl_metal.cpp
    src/gui.cpp
    src/renderer/platform/imgui_impl_sdl_bgfx.cpp
    src/renderer/renderer.cpp
    )

set(HEADERS 
    src/app.h
    src/gui.h
    src/renderer/platform/imgui_impl_sdl_bgfx.h
    src/renderer/renderer.h
    )

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      bgfx
                      ImGui
                      SDL2-static
                      )

# target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_compile_definitions(${PROJECT_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS" "-DENTRY_CONFIG_IMPLEMENT_MAIN=1" )
# target_compile_definitions(${PROJECT_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS")


if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:\"mainCRTStartup\"" )
endif()

