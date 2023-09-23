#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>

#include <glad/glad.h>

#include "io/file.h"
#include "allocator.h"
#include "types.h"
#include "logging.h"
#include "shader.h"

gl_obj shader_compile(allocator_t allocator, const char* path, GLenum shader_type) {
    u8* shader_source = file_load(allocator, path);
    if (shader_source == NULL) {
        LOG_MSG(error, "failed to open GLSL source file %s\n", path);
        return 0;
    }
    gl_obj shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, (const GLchar* const *) &shader_source, NULL);
    glCompileShader(shader);
    
    // Check for shader compile errors
    s32 success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512] = {0};
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        LOG_MSG(error, "failed to compile shader %s.\n%s\n", path, log);
    }
    allocator.free(shader_source);
    return shader;
}

