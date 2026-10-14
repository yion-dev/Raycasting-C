cmake_minimum_required(VERSION 3.15)

project(raycaster-c LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME} main.c)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        include
        src
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 m)

add_custom_target(run
    COMMAND ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    USES_TERMINAL
)
