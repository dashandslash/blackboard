cmake_minimum_required(VERSION 3.21)
set(PROJECT_NAME blackboard)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FetchContent)

FetchContent_Declare(
    EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG master
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(EnTT)

FetchContent_Declare(
    glm
    GIT_REPOSITORY git@github.com:g-truc/glm.git
    GIT_TAG master
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    range-v3
    GIT_REPOSITORY git@github.com:ericniebler/range-v3.git
    GIT_TAG master
    GIT_SHALLOW 1
)
FetchContent_Populate(range-v3)
add_subdirectory(${range-v3_SOURCE_DIR} ${range-v3_BINARY_DIR})
include_directories(${range-v3_SOURCE_DIR}/include)

include_directories(external)
include_directories(external/imgui)
include_directories(external/bgfx.cmake/bx/include)
include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(example)
add_subdirectory(core)
add_subdirectory(external)
