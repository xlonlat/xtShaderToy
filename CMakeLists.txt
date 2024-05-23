cmake_minimum_required(VERSION 3.14)

project(xtShaderToy)

### Source Files
set(SOURCES
    ../src/glad/glad.c
    ../src/xtShaderToy.cpp
    ../src/main.cpp
)

### Output
add_executable(${PROJECT_NAME} ${SOURCES})

### Header Dirs
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/)

### Libraries
find_package(glfw3 REQUIRED )
target_link_libraries(${PROJECT_NAME} dl GL glfw)
