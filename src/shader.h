#pragma once
#include <stdbool.h>

#include <glad/glad.h>

#include "types.h"

bool shader_link_check(gl_obj shader);

gl_obj shader_compile_src(const char* src, GLenum shader_type);
gl_obj shader_compile(const char* path, GLenum shader_type);

