#pragma once
#include <glad/glad.h>

#include "allocator.h"
#include "types.h"

gl_obj shader_compile(allocator_t allocator, const char* path, GLenum shader_type);

