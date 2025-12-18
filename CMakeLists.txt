cmake_minimum_required(VERSION 3.20)

project(VibeSim)

find_package(SFML 2.6 REQUIRED COMPONENTS graphics window system)

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/components/*.cpp
    src/styles/*.cpp
    src/utils/*.cpp
    src/simulation/*.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(VibeSim PRIVATE sfml-graphics sfml-window sfml-system)

if (WIN32)
    add_custom_command(TARGET VibeSim POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-graphics>
        $<TARGET_FILE:sfml-window>
        $<TARGET_FILE:sfml-system>
        $<TARGET_FILE_DIR:VibeSim>
    )
endif()

if (APPLE)
    set_target_properties(VibeSim PROPERTIES
        BUILD_RPATH "@executable_path"
        INSTALL_RPATH "@executable_path"
    )

    add_custom_command(TARGET VibeSim POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-graphics>
        $<TARGET_FILE:sfml-window>
        $<TARGET_FILE:sfml-system>
        $<TARGET_FILE_DIR:VibeSim>
    )
endif()

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})