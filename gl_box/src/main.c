#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Windows-only (for Sleep())
// #include <synchapi.h>

#include "types.h"
#include "shader.h"

vec3f quad_vertices[] = {
        { 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f},
        { 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f},
        {-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f},
        {-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}
};

uint32_t quad_indices[] = {
        0, 1, 3,
        1, 2, 3
};

void frame_resize_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // Remove the console window on startup on Windows
    // FreeConsole();

    static const int32_t width = 800;
    static const int32_t height = 600;

    // Setup GLFW with OpenGL Core 3.3
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(width, height, "Hello Triangle", NULL, NULL);
    if (window == NULL) {
        printf("main(): Failed to create GLFW window of size %dx%d.\n", width, height);
        glfwTerminate();
        return 1;
    }

    // Create the OpenGL context
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("main(): Failed to initialize GLAD for OpenGL Core 3.3.\n");
        return 1;
    }
    // Set OpenGL viewport to size of window, handle resizing
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, frame_resize_callback);

    // Setup VAO to store our state
    gl_obj vertex_array = 0;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    // Setup vertex buffer
    gl_obj vertex_buffer = 0;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

    // Element buffer
    gl_obj element_buffer = 0;
    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), &quad_indices, GL_STATIC_DRAW);

    // Create vertex layout
    glVertexAttribPointer(0, sizeof(vec3f) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(vec3f) * 2, NULL);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, sizeof(vec3f) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(vec3f) * 2, (void*)sizeof(vec3f));
    glEnableVertexAttribArray(1);

    // Load and compile shaders
    char vert_path[256] = CMAKE_SRC_ROOT;
    strcat(vert_path, "/gl_box/src/gl/vertex.glsl");
    gl_obj vertex_shader = compile_shader(vert_path, GL_VERTEX_SHADER);

    char frag_path[256] = CMAKE_SRC_ROOT;
    strcat(frag_path, "/gl_box/src/gl/fragment.glsl");
    gl_obj fragment_shader = compile_shader(frag_path, GL_FRAGMENT_SHADER);

    if (vertex_shader == 0 || fragment_shader == 0) {
        printf("main(): Failed to compile shaders.\n");
    }
    else {
        // Link the compiled shaders
        gl_obj shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);

        // Linker error checking
        int link_success = 0;
        glGetProgramiv(shader_program, GL_LINK_STATUS, &link_success);
        if (!link_success) {
            char log[512] = {0};
            glGetProgramInfoLog(shader_program, sizeof(log), NULL, log);
            printf("main(): Failed to link shader program.\n%s\n", log);
        }
        glUseProgram(shader_program);

        // Delete the individual shaders
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Keep window alive and updated
        while (!glfwWindowShouldClose(window)) {

            // Windows-only
            // Sleep(16);

            // Clear framebuffer
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Draw
            glUseProgram(shader_program);
            glBindVertexArray(vertex_array);
            glDrawElements(GL_TRIANGLES, sizeof(quad_indices) / sizeof(*quad_indices), GL_UNSIGNED_INT, 0);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        // Clean up
        glDeleteProgram(shader_program);
        glDeleteVertexArrays(1, &vertex_array);
        glDeleteBuffers(1, &vertex_buffer);
    }

    // Shut down
    glfwTerminate();
    return 0;
}
