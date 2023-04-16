cmake_minimum_required(VERSION 3.16)
project(gl_box C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(ext/glad)
add_subdirectory(ext/glfw)
add_subdirectory(gl_box)

target_include_directories(gl_box PRIVATE "ext/glfw" "ext/glew/")
