#include <glad/glad.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>

#include "logging.h"

void gl_debug_msg(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam) {
  char* source_str = NULL;
  switch (source) {
      case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
          source_str = "shader compiler";
          break;
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
          source_str = "windowing system";
          break;
      case GL_DEBUG_SOURCE_API_ARB:
          source_str = "API";
          break;
      case GL_DEBUG_SOURCE_APPLICATION_ARB:
          source_str = "application";
          break;
      case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
          source_str = "third-party";
          break;
      case GL_DEBUG_SOURCE_OTHER_ARB:
          source_str = "[other source]";
          break;
      default:
          source_str = "[unknown source]";
  }

  switch (severity) {
      case GL_DEBUG_SEVERITY_HIGH_ARB:
        LOG_MSG(error, "OpenGL %s message [CRITICAL] ", source_str);
        break;
      case GL_DEBUG_SEVERITY_MEDIUM_ARB:
        LOG_MSG(warning, "OpenGL %s message [WARNING] ", source_str);
        break;
      case GL_DEBUG_SEVERITY_LOW_ARB:
        LOG_MSG(info, "OpenGL %s message [LOW] ", source_str);
        break;
      default:
        LOG_MSG(debug, "OpenGL %s message [DEBUG] ", source_str);
  }
  printf("[id %d]: %s\n", id, message);
}

void gl_debug_setup() {
    if (!glfwExtensionSupported("GL_ARB_debug_output")) {
        LOG_MSG(warning, "Missing GL_ARB_debug_output, debug messages will be missing.\n");
        return;
    }
    else {
        LOG_MSG(debug, "Enabling OpenGL debug messages\n");
        int flags = 0;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallbackARB(gl_debug_msg, NULL);
            // No message filtering, we print EVERYTHING.
            glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
        }
    }
}
