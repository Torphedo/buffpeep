#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>

#include <glad/glad.h>

#include "shader.h"

// Read an entire file into a buffer. Caller is responsible for freeing the resource.
// This is mainly used to load shader source code.
uint8_t* load_resource(const char* path) {
    struct stat st = {0};
    bool exists = (stat(path, &st) == 0);
    if (exists) {
        uint8_t* buffer = calloc(1, st.st_size);
        FILE* resource = fopen(path, "rb");
        if (resource != NULL && buffer != NULL) {
            fread(buffer, st.st_size, 1, resource);
            fclose(resource);
            return buffer;
        }
    }
    printf("load_resource(): %s doesn't exist.\n", path);
    return NULL;
}

gl_obj compile_shader(const char* path, GLenum shader_type) {
    char* shader_source = (char*)load_resource(path);
    if (shader_source == NULL) {
        printf("compile_shader(): Failed to open GLSL source file %s\n", path);
        return 0;
    }
    gl_obj shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, (const GLchar* const *) &shader_source, NULL);
    glCompileShader(shader);

    // Check for shader compile errors
    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512] = {0};
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        printf("compile_shader(): Failed to compile shader.\n%s\n", log);
    }
    free(shader_source);
    return shader;
}
