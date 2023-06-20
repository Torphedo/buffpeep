cmake_minimum_required(VERSION 3.16)
project(gl_box C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(ext/cglm)
option(CGLM_SHARED "Shared build" OFF)
option(CGLM_STATIC "Static build" ON)
option(CGLM_USE_C99 "" OFF)
option(CGLM_USE_TEST "Enable Tests" OFF) # for make check - make test

add_subdirectory(ext/glad)
add_subdirectory(ext/glfw)
add_subdirectory(gl_box)

target_include_directories(gl_box PRIVATE "ext/glfw" "ext/glew/" "ext/cglm")
target_compile_definitions(gl_box PRIVATE CMAKE_SRC_ROOT="${CMAKE_SOURCE_DIR}")

file(COPY "data/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/gl_box/data)

