cmake_minimum_required(VERSION 3.21)
project(blackboard_example)

include(FetchContent)

FetchContent_Declare(
    EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG master
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(EnTT)

set(SOURCES
    example.cpp
    )

set(HEADERS 
    )

if(WIN32)
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:\"mainCRTStartup\"" )
elseif(APPLE)
add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES
    BUNDLE True
    RESOURCE true
    MACOSX_BUNDLE_GUI_IDENTIFIER ll.dashandslash.${PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS")

target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      core
                      EnTT
                      )

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_LIST_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/assets
)

add_dependencies(${PROJECT_NAME} copy_assets)
