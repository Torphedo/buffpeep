#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <cglm/struct.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gl_setup.h"
#include "viewer.h"
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

int main(int argc, char** argv) {
    // Remove the console window on startup on Windows
    // FreeConsole();
    if (argc != 2) {
        LOG_MSG(error, "Please provide a resource filename.\n")
        return 1;
    }
    u8* img_buf = image_buf_load(argv[1]);
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
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
    // Enable transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // Load texture
    u32 gl_img = 0;
    glGenTextures(1, &gl_img);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gl_img);

    // Wrapping & filtering settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    LOG_MSG(info, "Binding loaded image data.\n");
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, img.width, img.height, 0, (img.width * img.height) / 2, img.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    printf("\n");
    mat4s view = glms_mat4_identity(); // Create identity matrix

    // Keep window alive and updated
    while (!glfwWindowShouldClose(window) && !input.q) {
        // Manages active texture's format, dimensions, etc.
        viewer_update(&img);

        // Clear framebuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int cur_width = 0;
        int cur_height = 0;
        glfwGetWindowSize(window, &cur_width, &cur_height);

        // Quad transforms (updated each frame)

        float ratio = (float)img.width / (float)img.height;
        gl_obj u_ratio = glGetUniformLocation(shader_program, "ratio");
        glUniform1f(u_ratio, ratio);

        mat4s model = glms_mat4_identity(); // Create identity matrix
        model = glms_rotate(model, glm_rad(180.0f), (vec3s){1.0f, 0.0f, 0.0f});
        model = glms_scale(model, (vec3s){(float)cur_height / (float)(cur_width), 1.0f, 1.0f}); // half screen
        // model = glms_scale(model, (vec3s){0.5f, 1.0f, 1.0f}); // correct size
        // model = glms_scale(model, (vec3s){0.5625f * 0.5f, 1.0f, 1.0f}); // correct size
        // glm_rotate(model, glfwGetTime(), (vec3){0.0f, 0.0f, 1.0f});
        gl_obj u_model = glGetUniformLocation(shader_program, "model");
        glUniformMatrix4fv(u_model, 1, GL_FALSE, (const float*)&model.raw);

        view = viewer_update_camera(view);
        gl_obj u_view = glGetUniformLocation(shader_program, "view");
        glUniformMatrix4fv(u_view, 1, GL_FALSE, (const float*)&view.raw);

        // Draw
        glUseProgram(shader_program);
        glBindVertexArray(vertex_array);
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

