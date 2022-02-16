cmake_minimum_required(VERSION 3.21)
set(PROJECT_NAME blackboard)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_NAME})

add_subdirectory(example)
add_subdirectory(${PROJECT_NAME})
