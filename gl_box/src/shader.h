#pragma once
#include <glad/glad.h>

#include "allocator.h"
#include "types.h"

gl_obj compile_shader(allocator_t allocator, const char* path, GLenum shader_type);
