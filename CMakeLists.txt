cmake_minimum_required(VERSION 3.21)
set(PROJECT_NAME blackboard)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(external)
include_directories(external/imgui)
include_directories(external/bgfx.cmake/bx/include)
include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(example)
add_subdirectory(core)
add_subdirectory(external)
add_subdirectory(engine_meta)
