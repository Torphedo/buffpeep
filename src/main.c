#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <cglm/struct.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <common/gl/gl_setup.h>
#include <common/gl/input.h>
#include <common/int.h>
#include <common/image.h>
#include <common/logging.h>
#include "viewer.h"
#include "render_image.h"

// 32MiB image buffer, just so we never run out of scratch space. Last I
// checked the window & OpenGL context was ~100MiB, so this isn't a big deal.
static u8 img_buf[0x2000000] = {0};

int main(int argc, char** argv) {
    // Print in color on Windows
    enable_win_ansi();

    if (argc != 2) {
        LOG_MSG(error, "Please provide a resource filename.\n");
        return 1;
    }
    // Load image data with our best guess of metadata
    texture img = image_buf_load(argv[1], img_buf, sizeof(img_buf));

    // This automatically sets up all the input handling as well
    static const s32 width = 800;
    static const s32 height = 600;
    GLFWwindow* window = setup_opengl(width, height, "buffpeep", ENABLE_DEBUG, 0, true);
    if (window == NULL) {
        LOG_MSG(error, "Failed to create a valid window & OpenGL context for rendering\n");
        return 1;
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

