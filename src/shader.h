#pragma once
#include <stdbool.h>

#include <glad/glad.h>

#include "allocator.h"
#include "types.h"

bool shader_link_check(gl_obj shader);

gl_obj shader_compile(allocator_t allocator, const char* path, GLenum shader_type);

