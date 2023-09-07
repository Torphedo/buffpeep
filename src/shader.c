#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>

#include <glad/glad.h>

#include "allocator.h"
#include "types.h"
#include "logging.h"
#include "shader.h"

// Read an entire file into a buffer. Caller is responsible for freeing the resource.
// This is mainly used to load shader source code.
u8* load_resource(allocator_t allocator, const char* path) {
    struct stat st = {0};
    bool exists = (stat(path, &st) == 0);
    if (exists) {
        u8* buffer = allocator.calloc(1, st.st_size);
        FILE* resource = fopen(path, "rb");
        if (resource != NULL && buffer != NULL) {
            fread(buffer, st.st_size, 1, resource);
            fclose(resource);
            return buffer;
        }
    }
    LOG_MSG(error, "%s doesn't exist.\n", path);
    return NULL;
}

gl_obj compile_shader(allocator_t allocator, const char* path, GLenum shader_type) {
    char* shader_source = (char*)load_resource(allocator, path);
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

