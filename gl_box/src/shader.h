#pragma once
#include <glad/glad.h>

#include "types.h"

gl_obj compile_shader(const char* path, GLenum shader_type);
