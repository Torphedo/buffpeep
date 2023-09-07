cmake_minimum_required(VERSION 3.16)
project(gl_box C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(ext/glad)
add_subdirectory(ext/glfw)
add_subdirectory(src)

target_include_directories(gl_box PRIVATE
  "ext/glfw"
  "ext/glad/"
  "ext/cglm/include"
)
target_compile_definitions(gl_box PRIVATE CMAKE_SRC_ROOT="${CMAKE_SOURCE_DIR}")

file(COPY "data/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/src/data)

