cmake_minimum_required(VERSION 3.20)

project(Servicio)

find_package(SFML 2.6 REQUIRED COMPONENTS graphics window system)

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/components/*.cpp
    src/styles/*.cpp
    src/utils/*.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(Servicio PRIVATE sfml-graphics sfml-window sfml-system)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})