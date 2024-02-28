#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#include <cglm/struct.h>
#include <glad/glad.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>

#include "gl_debug.h"
#include "gl_setup.h"
#include "types.h"
#include "shader.h"
#include "image.h"
#include "logging.h"
#include "input.h"

vertex quad_vertices[] = {
    { .position = {0.5f, 0.5f, 0.0f},
      .tex_coord = {1.0f, 1.0f}
    },
    {
      .position = {0.5f, -0.5f, 0.0f},
      .tex_coord = {1.0f, 0.0f}
    },
    {
      .position = {-0.5f, -0.5f, 0.0f},
      .tex_coord = {0.0f, 0.0f}
    },
    {
      .position = {-0.5f,  0.5f, 0.0f},
      .tex_coord = {0.0f, 1.0f}
    },
    { .position = {0.5f, 0.5f, 0.0f},
      .tex_coord = {1.0f, 1.0f}
    },
    {
      .position = {-0.5f, -0.5f, 0.0f},
      .tex_coord = {0.0f, 0.0f}
    }
};

int main() {
    // Remove the console window on startup on Windows
    // FreeConsole();
    u8* img_buf = image_buf_load("data/resource_0.bin");
    texture img = {
        .data = img_buf,
        .width = 512,
        .height = 64,
        .mip_level = 1,
        .bits_per_pixel = 4
    };

    static const s32 width = 800;
    static const s32 height = 600;
    GLFWwindow* window = setup_opengl(width, height, "gl_box", ENABLE_DEBUG);
    if (window == NULL) {
        LOG_MSG(error, "Failed to create a valid window & OpenGL context for rendering\n");
        return 1;
    }

    glfwSetKeyCallback(window, input_update);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) {
      glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    
    // Setup VAO to store our state
    gl_obj vertex_array = 0;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    
    // Setup vertex buffer
    gl_obj vertex_buffer = 0;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    
    // Create vertex layout
    glVertexAttribPointer(0, sizeof(vec3f) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, sizeof(vec2f) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, tex_coord));
    glEnableVertexAttribArray(1);
    
    // Load and compile shaders
    char vert_path[256] = SRC_ROOT;
    strncat(vert_path, "/src/gl/vertex.glsl", sizeof(vert_path) -1);
    gl_obj vertex_shader = shader_compile(vert_path, GL_VERTEX_SHADER);
    
    char frag_path[256] = SRC_ROOT;
    strncat(frag_path, "/src/gl/fragment.glsl", sizeof(frag_path) - 1);
    gl_obj fragment_shader = shader_compile(frag_path, GL_FRAGMENT_SHADER);
    
    if (vertex_shader == 0 || fragment_shader == 0) {
        // All needed error information should already be given by the shader
        // compiler function and our debug callbacks.
        LOG_MSG(error, "failed to compile shaders.\n");
        glfwTerminate();
        return 1;
    }

    // Link the compiled shaders
    gl_obj shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Make sure linking succeeded
    if (!shader_link_check(shader_program)) {
        // No need to print, link check prints messages on failure.
        return 1;
    }
    glUseProgram(shader_program);

    // Delete the individual shader objects
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);


    glEnable(GL_DEPTH_TEST);

    // Load texture
    u32 gl_img = 0;
    glGenTextures(1, &gl_img);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gl_img);

    // Wrapping & filtering settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    LOG_MSG(info, "Binding loaded image data.\n");
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, img.width, img.height, 0, (img.width * img.height) / 2, img.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    bool compressed = true;
    u8 img_fmt = DXT5;

    // For raw formats
    u8 unit_size = 0; // 0 = u8, 1 = u16, 2 = u32
    u8 channels = 4;
    bool up_last_frame = false;
    bool down_last_frame = false;
    bool spc_last_frame = false;
    printf("\n"); // Go up a line

    // Keep window alive and updated
    while (!glfwWindowShouldClose(window)) {
        bool up = input.k || input.up;
        bool down = input.j || input.down;
        bool changed = up || down || input.space || input.left || input.right;
        compressed ^= input.space && !spc_last_frame;
        spc_last_frame = input.space;
        if (changed) {
            printf("\033[1F\033[2K"); // Go up a line

            u32 res = (img.height * img.width);
            if (compressed) {
                img_fmt += up && !up_last_frame;
                img_fmt -= down && !down_last_frame;
                img_fmt %= 3;

                GLenum format = 0;
                u32 size = res;
                switch (img_fmt) {
                    case DXT3:
                        LOG_MSG(info, "DXT3");
                        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                        break;
                    case DXT5:
                        LOG_MSG(info, "DXT5");
                        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                        break;
                    default:
                        LOG_MSG(info, "DXT1");
                        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                        size /= 2;
                        break;
                };
                glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, img.width, img.height, 0, size, img.data);
            }
            else {
                unit_size += up && !up_last_frame;
                unit_size -= down && !down_last_frame;
                unit_size %= 3;

                channels += input.right;
                channels -= input.left;
                channels %= 5;
                LOG_MSG(info, "unit size = %d bytes, %d channels", 1 << unit_size, channels);
                if (changed) {
                    GLenum gl_size = GL_UNSIGNED_BYTE + (unit_size * 2);
                    GLenum format = GL_RGBA;
                    glTexImage2D(GL_TEXTURE_2D, 0, format, img.width, img.height, 0, format, gl_size, img.data);
                }
            }

            glGenerateMipmap(GL_TEXTURE_2D);
            printf("\n");
        }
        down_last_frame = down;
        up_last_frame = up;

        // Clear framebuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Quad transforms (updated each frame)
        mat4 model = {0};
        glm_mat4_identity(model); // Create identity matrix
        glm_rotate(model, glm_rad(180.0f), (vec3){1.0f, 0.0f, 0.0f});
        glm_translate(model, (vec3){-0.5f, -0.5f, 0.0f});
        // glm_rotate(model, glfwGetTime(), (vec3){0.0f, 0.0f, 1.0f});
        gl_obj u_model = glGetUniformLocation(shader_program, "model");
        glUniformMatrix4fv(u_model, 1, GL_FALSE, (const float*)&model);

        // Draw
        glUseProgram(shader_program);
        glBindTexture(GL_TEXTURE_2D, gl_img);
        glBindVertexArray(vertex_array);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(quad_vertices) / sizeof(*quad_vertices));

        // Quad transforms (updated each frame)
        // glm_rotate(model, glm_rad(29.0f), (vec3){0.7f, 1.0f, 0.2f});
        glm_translate(model, (vec3){1.0f, 0.0f, 0.0f});
        glUniformMatrix4fv(u_model, 1, GL_FALSE, (const float*)&model);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(quad_vertices) / sizeof(*quad_vertices));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Clean up
    glDeleteProgram(shader_program);
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteBuffers(1, &vertex_buffer);
    
    // Shut down
    glfwTerminate();
    return 0;
}

