cmake_minimum_required(VERSION 3.16)
project(glad C)

set(CMAKE_C_STANDARD 99)

add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC include)