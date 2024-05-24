#!/bin/sh
zig cc -target x86_64-windows -O2 src/*.c ext/glad/src/*.c ext/glfw/src/*.c -Iext/cglm/include -Iext/glad/include -Iext/glfw/include -DGLFW_BUILD_WIN32 -D_GLFW_WIN32 -lgdi32 -obuffpeep.exe

