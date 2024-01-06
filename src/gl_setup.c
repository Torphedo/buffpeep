#include <stdbool.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "logging.h"
#include "input.h"
#include "types.h"
#include "gl_debug.h"

void frame_resize_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void glfw_error(int err_code, const char* msg) {
    // Saying "GLFW error" in the message is redundant because this function
    // name is printed as part of LOG_MSG().
    LOG_MSG(error, "[code %d] %s\n", err_code, msg);
}


GLFWwindow* setup_opengl(s32 width, s32 height, const char* window_name, bool enable_debug) {
    glfwSetErrorCallback(glfw_error);

    // Setup GLFW
    if (!glfwInit()) {
        return NULL;
    }

    // Use OpenGL Core 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Request a debug context as determined by caller
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, enable_debug);
    
    GLFWwindow* window = glfwCreateWindow(width, height, window_name, NULL, NULL);
    if (window == NULL) {
        LOG_MSG(error, "failed to create GLFW window of size %dx%d.\n", width, height);
        glfwTerminate();
        return window;
    }

    // Start tracking input state & using virtual cursor positions.
    glfwSetKeyCallback(window, input_update);
    glfwSetCursorPosCallback(window, cursor_update);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scroll_update);

    // Get non-accelerated input if possible
    LOG_MSG(info, "Raw mouse motion ");
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        printf("enabled.\n");
    }
    else {
        printf("disabled.\n");
    }
    
    // Create the OpenGL context
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        LOG_MSG(error, "failed to initialize GLAD for OpenGL Core 3.3.\n");
        glfwTerminate();
        return NULL;
    }
    
    if (enable_debug) {
        gl_debug_setup();
        // Example code that will trigger a critical debug message:
        // glBindBuffer(GL_VERTEX_ARRAY_BINDING, 0);
    }

    // Set OpenGL viewport to size of window, handle resizing
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, frame_resize_callback);

    return window;
}

