cmake_minimum_required(VERSION 3.28)
project(project_SO)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(include/imgui)

file(GLOB GLFW_SOURCES src/glfw-3.4/include/GLFW/*.h)
file(GLOB IMGUI_SOURCES src/imgui/*.cpp)
file(GLOB SOURCES src/main.cpp ${IMGUI_SOURCES} ${GLFW_SOURCES})

find_package(OpenGL REQUIRED)

add_subdirectory(src/glfw-3.4)

add_executable(project_SO ${SOURCES})
target_link_libraries(project_SO ${OPENGL_LIBRARIES} glfw)