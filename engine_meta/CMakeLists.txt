cmake_minimum_required(VERSION 3.21)
project(engine_meta)

include(FetchContent)

FetchContent_Declare(
    EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG master
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(EnTT)

    FetchContent_Declare(
        concurrentqueue
        GIT_REPOSITORY https://github.com/cameron314/concurrentqueue.git
        GIT_TAG master
        GIT_SHALLOW 1
    )
    FetchContent_MakeAvailable(concurrentqueue)

set(SOURCES
    main.cpp
    action_a.h
    action_b.h
    executor_a.h
    executor_b.h
    module.h
    module.cpp
    )

set(HEADERS 
    )

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      core
                      EnTT
                      concurrentqueue
                      )

target_compile_definitions(${PROJECT_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS")

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:\"mainCRTStartup\"" )
endif()

