cmake_minimum_required(VERSION 3.16)
project(buffpeep C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(ext/glad)
add_subdirectory(ext/glfw)

add_executable(buffpeep
        "src/main.c"
        "src/shader.c"
        "src/file.c"
        "src/image.c"
        "src/gl_setup.c"
        "src/input.c"
        "src/logging.c"
        "src/viewer.c"
)
target_link_libraries(buffpeep PRIVATE glfw glad)

target_include_directories(buffpeep PRIVATE
  "ext/glfw"
  "ext/glad"
  "ext/cglm/include"
)
target_compile_definitions(buffpeep PRIVATE SRC_ROOT="${CMAKE_SOURCE_DIR}")

file(COPY "src/data/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
file(COPY "src/gl/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

