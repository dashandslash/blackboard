cmake_minimum_required(VERSION 3.21)

include(FetchContent)
set(FETCHCONTENT_QUIET off)

set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.external)

# SDL
FetchContent_Declare(
			               sdl
                     GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
			               GIT_TAG release-2.0.20
                     GIT_SHALLOW 1
)
FetchContent_GetProperties(sdl)
if(NOT sdl_POPULATED)
  FetchContent_Populate(sdl)

  if(APPLE)
    set(SDL_VIDEO_METAL 1)
	endif()

	add_subdirectory(${sdl_SOURCE_DIR} ${sdl_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# imgui
FetchContent_Declare(
                     imgui
                     GIT_REPOSITORY https://github.com/ocornut/imgui.git
                     GIT_TAG docking
                     GIT_SHALLOW 1
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
                     imguizmo
                     GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
                     GIT_TAG master
                     GIT_SHALLOW 1
)
FetchContent_MakeAvailable(imguizmo)


# FetchContent_Declare(
#                      imgui_color_text_edit
#                      GIT_REPOSITORY https://github.com/BalazsJako/ImGuiColorTextEdit.git
#                      GIT_TAG master
#                      GIT_SHALLOW 1
# )
# FetchContent_MakeAvailable(imgui_color_text_edit)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${SDL2_SOURCE_DIR}/include ${FETCHCONTENT_BASE_DIR}/SDL 
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${imgui_SOURCE_DIR} ${FETCHCONTENT_BASE_DIR}/imgui
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${imguizmo_SOURCE_DIR} ${FETCHCONTENT_BASE_DIR}/imguizmo
)

set(imgui_SOURCE_SYMLINK_DIR ${FETCHCONTENT_BASE_DIR}/imgui)

file(GLOB imguizmo_files
     "${imguizmo_SOURCE_DIR}/*.h"
     "${imguizmo_SOURCE_DIR}/*.cpp"
)

message(STATUS "imguizmo_files: ${imguizmo_files}")

set(imgui_SOURCES
  ${imgui_SOURCE_SYMLINK_DIR}/imgui.cpp
  ${imgui_SOURCE_SYMLINK_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_SYMLINK_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_SYMLINK_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_SYMLINK_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_SYMLINK_DIR}/misc/cpp/imgui_stdlib.cpp
  ${imgui_SOURCE_SYMLINK_DIR}/backends/imgui_impl_sdl.cpp
  # ${imgui_color_text_edit_SOURCE_DIR}/TextEditor.cpp
  )

set(imgui_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/blackboard_core/gui/imconfig.h
  ${imgui_SOURCE_SYMLINK_DIR}/imgui.h
  ${imgui_SOURCE_SYMLINK_DIR}/imstb_rectpack.h
  ${imgui_SOURCE_SYMLINK_DIR}/imstb_truetype.h
  ${imgui_SOURCE_SYMLINK_DIR}/imgui_internal.h
  ${imgui_SOURCE_SYMLINK_DIR}/imstb_textedit.h
  ${imgui_SOURCE_SYMLINK_DIR}/backends/imgui_impl_sdl.h
  ${imgui_SOURCE_SYMLINK_DIR}/misc/cpp/imgui_stdlib.h
  # ${imgui_color_text_edit_SOURCE_DIR}/TextEditor.h
  )

add_library(ImGui STATIC ${imgui_SOURCES} ${imgui_HEADERS} ${imguizmo_files})

target_link_libraries(ImGui
                      PUBLIC
                      SDL2-static
)

target_include_directories(ImGui
                           PUBLIC
                           $<BUILD_INTERFACE:${imgui_SOURCE_SYMLINK_DIR}>
)

include_directories(${FETCHCONTENT_BASE_DIR})

# EnTT
FetchContent_Declare(
                     EnTT
                     GIT_REPOSITORY https://github.com/skypjack/entt.git
                     GIT_TAG master
                     GIT_SHALLOW 1
)
FetchContent_MakeAvailable(EnTT)

# glm
FetchContent_Declare(
                     glm
                     GIT_REPOSITORY https://github.com/g-truc/glm.git
                     GIT_TAG master
                     GIT_SHALLOW 1
)
FetchContent_MakeAvailable(glm)

# bgfx_cmake
FetchContent_Declare(
                     bgfx_cmake
                     GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake.git
                     GIT_TAG master
                     GIT_SHALLOW 1
)
FetchContent_GetProperties(bgfx_cmake)
if(NOT bgfx_cmake_POPULATED)
	FetchContent_Populate(bgfx_cmake)
	if(EMSCRIPTEN OR IOS)
    		set(BGFX_BUILD_TOOLS OFF CACHE INTERNAL "")
	else()
    		set(BGFX_BUILD_TOOLS ON CACHE INTERNAL "")
	endif()
	set( BGFX_BUILD_EXAMPLES  OFF CACHE INTERNAL "" )
	set( BGFX_CUSTOM_TARGETS  OFF CACHE INTERNAL "" )
	add_subdirectory(${bgfx_cmake_SOURCE_DIR} ${bgfx_cmake_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# assimp
FetchContent_Declare(
                     assimp
                     GIT_REPOSITORY https://github.com/assimp/assimp.git
                     GIT_TAG v5.2.2
                     GIT_SHALLOW 1
)
FetchContent_GetProperties(assimp)

if(NOT assimp_POPULATED)
	FetchContent_Populate(assimp)

  set(ASSIMP_COVERALLS OFF CACHE INTERNAL "" FORCE)
  set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE INTERNAL "" FORCE)
  set(ASSIMP_BUILD_SAMPLES OFF CACHE INTERNAL "" FORCE)
  set(ASSIMP_DOUBLE_PRECISION OFF CACHE INTERNAL "" FORCE)
  set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
  set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
  set(ASSIMP_INSTALL_PDB OFF CACHE INTERNAL "" FORCE)
  set(IGNORE_GIT_HASH OFF CACHE INTERNAL "" FORCE)
  set(INJECT_DEBUG_POSTFIX OFF CACHE INTERNAL "" FORCE )
  set(ASSIMP_INSTALL OFF CACHE INTERNAL "" FORCE)

  if(APPLE)
  set(BUILD_FRAMEWORK ON CACHE INTERNAL "" FORCE)
  endif()

  

	add_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()


# FetchContent_MakeAvailable(assimp)