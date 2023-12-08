#include <stdbool.h>

#include <GLFW/glfw3.h>

#include "types.h"

typedef enum {
    DISABLE_DEBUG = false,
    ENABLE_DEBUG = true
}enable_debug_msg;

// Setup GLFW window and create an OpenGL 3.3 context on core profile.
// If requested, a debug context will be created with a debug message callback
// (if provided as an extension by the driver).
GLFWwindow* setup_opengl(s32 width, s32 height, const char* window_name, bool enable_debug);

