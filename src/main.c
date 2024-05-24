#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <cglm/struct.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gl_setup.h"
#include "viewer.h"
#include "types.h"
#include "render_image.h"
#include "image.h"
#include "logging.h"
#include "input.h"

int main(int argc, char** argv) {
    // Print in color on Windows
    #ifdef _WIN32
    enable_win_ansi();
    #endif

    if (argc != 2) {
        LOG_MSG(error, "Please provide a resource filename.\n");
        return 1;
    }
    texture img = image_buf_load(argv[1]);

    static const s32 width = 800;
    static const s32 height = 600;
    GLFWwindow* window = setup_opengl(width, height, "buffpeep", ENABLE_DEBUG);
    if (window == NULL) {
        LOG_MSG(error, "Failed to create a valid window & OpenGL context for rendering\n");
        return 1;
    }

    glfwSetKeyCallback(window, input_update);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    void* img_ctx = image_renderer.init(&img); 
    if (img_ctx == NULL) {
        glfwTerminate();
        return 1;
    }

    printf("\n");
    // Keep window alive and updated
    while (!glfwWindowShouldClose(window) && !input.q) {
        // Clear framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        image_renderer.render(img_ctx, window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    image_renderer.destroy(img_ctx);
    
    // Shut down
    glfwTerminate();
    return 0;
}

