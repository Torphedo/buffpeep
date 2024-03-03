#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>

#include <glad/glad.h>

#include "file.h"
#include "types.h"
#include "logging.h"
#include "shader.h"

// Linker error checking
bool shader_link_check(gl_obj shader) {
    s32 link_success = 0;
    glGetProgramiv(shader, GL_LINK_STATUS, &link_success);
    if (link_success) {
        return true;
    }

    char log_buf[1024] = {0};
    LOG_MSG(error, "failed to link shader program. Linker log_buf: \n");

    s32 log_size = 0;
    glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &log_size);
    
    if (log_size > sizeof(log_buf)) {
        LOG_MSG(warning, "Abnormally large log (%d bytes) will be cut off at %d characters.\n", log_size, sizeof(log_buf));
        LOG_MSG(info, "Uncomment heap allocation code to allow full log.\n");
    }

    /*
    if (log_size > sizeof(log_buf)) {
        char* log_buf = calloc(1, log_size);
        glGetProgramInfoLog(shader, log_size, NULL, log_buf);
        printf("%s\n", log_buf);
        LOG_MSG(warning, "Abnormally large log_buf (%d bytes) forced a heap allocation.\n", log_size);
        free(log_buf);
    }
    */

    // Use stack array to print the log_buf.
    glGetProgramInfoLog(shader, sizeof(log_buf), NULL, log_buf);
    printf("%s\n", log_buf);
    return false;
}

gl_obj shader_compile_src(const char* src, GLenum shader_type) {
    gl_obj shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, (const GLchar* const *) &src, NULL);
    glCompileShader(shader);

    // Check for shader compile errors
    s32 success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512] = {0};
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        LOG_MSG(error, "failed to compile shader.\n%s\n", log);
    }
    return shader;
}

gl_obj shader_compile(const char* path, GLenum shader_type) {
    u8* shader_source = file_load(path);
    if (shader_source == NULL) {
        LOG_MSG(error, "failed to open GLSL source file %s\n", path);
        return 0;
    }
    gl_obj shader = shader_compile_src((char*)shader_source, shader_type);
    free(shader_source);
    return shader;
}

