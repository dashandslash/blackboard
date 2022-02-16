cmake_minimum_required(VERSION 3.21)

message("CMAKE_SYSTEM_NAME [${CMAKE_SYSTEM_NAME}]")

set(PROJECT_NAME blackboard_example)

project (${PROJECT_NAME})


set(SOURCES
    example.cpp
)

set(HEADERS 
)


if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    target_link_libraries(${PROJECT_NAME} "-s WASM=1") #web assembly FTW
    target_link_libraries(${PROJECT_NAME} "-s USE_SDL=2")
    target_link_libraries(${PROJECT_NAME} "-s USE_SDL_IMAGE=2")
    target_link_libraries(${PROJECT_NAME} "-s MIN_WEBGL_VERSION=2")
    target_link_libraries(${PROJECT_NAME} "-s MAX_WEBGL_VERSION=2")
    target_link_libraries(${PROJECT_NAME} "-s USE_WEBGL2=1")
    target_link_libraries(${PROJECT_NAME} "-s ASSERTIONS=1")
    target_link_libraries(${PROJECT_NAME} "-s ALLOW_MEMORY_GROWTH=1")
#    target_link_libraries(${PROJECT_NAME} "-s USE_SDL_IMAGE_FORMATS='[\"png\"]'")
    target_link_libraries(${PROJECT_NAME} "-o ${PROJECT_NAME}.html")
    target_link_libraries(${PROJECT_NAME} "-s DEMANGLE_SUPPORT=1")
    target_compile_options(${PROJECT_NAME} PUBLIC "-O2") #creates the runtime resources
    target_link_libraries(${PROJECT_NAME} "-O2") #creates the runtime resources
    target_link_libraries(${PROJECT_NAME} blackboard_lib)
    target_link_libraries(${PROJECT_NAME} bgfx)
    target_link_libraries(${PROJECT_NAME} EnTT)
    target_link_libraries(${PROJECT_NAME} glm)
#    target_link_libraries(${PROJECT_NAME} range-v3::range-v3)
elseif(WIN32)
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
#    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:\"mainCRTStartup\"" )
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

if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    
    target_link_libraries(${PROJECT_NAME}
                    PUBLIC
                    blackboard_lib
                    bgfx
                    bx
                    EnTT
                    glm
)

    target_compile_definitions(${PROJECT_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS")
    add_dependencies(${PROJECT_NAME} copy_assets)
    if(APPLE)
        add_custom_target(copy_assets
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_LIST_DIR}/assets"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/assets"
        )
    else()
        add_custom_target(copy_assets
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_LIST_DIR}/assets
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources/assets"
        )
    endif()

    if(APPLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            # Copy shaders
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/../blackboard/renderer/shaders"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/shaders"

            COMMAND ${CMAKE_COMMAND} -E copy

            "$<TARGET_FILE:shaderc>"

            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/tools/osx/shaderc"
        )
    else()
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            # Copy shaders
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/../blackboard/renderer/shaders"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources/shaders"

            COMMAND ${CMAKE_COMMAND} -E copy

            "$<TARGET_FILE:shaderc>"

            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources/tools/win/shaderc.exe"
        )
    endif()

else()
# copy the assets
endif()
