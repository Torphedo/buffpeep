#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#include <cglm/cglm.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Windows-only (for Sleep())
// #include <synchapi.h>

#include "cglm/affine-mat.h"
#include "cglm/affine-pre.h"
#include "cglm/affine.h"
#include "cglm/mat4.h"
#include "cglm/util.h"
#include "types.h"
#include "shader.h"
#include "image.h"
#include "logging.h"

typedef struct {
  vec3f position;
  vec3f color;
  vec2f tex_coord;
}vertex;

vertex quad_vertices[] = {
  { 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f},
  { 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f},
  {-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f},
  {-0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 1.0f}
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
    LOG_MSG(error, "failed to create GLFW window of size %dx%d.\n", width, height);
    glfwTerminate();
    return 1;
  }

  // Create the OpenGL context
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    LOG_MSG(error, "failed to initialize GLAD for OpenGL Core 3.3.\n");
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
  glVertexAttribPointer(0, sizeof(vec3f) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, sizeof(vec3f) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, sizeof(vec2f) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, tex_coord));
  glEnableVertexAttribArray(2);

  // Load and compile shaders
  char vert_path[256] = CMAKE_SRC_ROOT;
  strcat(vert_path, "/gl_box/src/gl/vertex.glsl");
  gl_obj vertex_shader = compile_shader(vert_path, GL_VERTEX_SHADER);

  char frag_path[256] = CMAKE_SRC_ROOT;
  strcat(frag_path, "/gl_box/src/gl/fragment.glsl");
  gl_obj fragment_shader = compile_shader(frag_path, GL_FRAGMENT_SHADER);

  if (vertex_shader == 0 || fragment_shader == 0) {
    LOG_MSG(error, "failed to compile shaders.\n");
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
      LOG_MSG(error, "failed to link shader program.\n%s\n", log);
    }
    glUseProgram(shader_program);

    // Delete the individual shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    

    // Load texture(s)
    texture perlin = load_dds(allocator_default, "data/container.dds");
    uint32_t gl_perlin = 0;
    glGenTextures(1, &gl_perlin);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gl_perlin);

    // Wrapping & filtering settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (perlin.data != NULL) {
      LOG_MSG(info, "Binding loaded image data.\n");
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, perlin.width, perlin.height, 0, GL_BGR, GL_UNSIGNED_BYTE, perlin.data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Keep window alive and updated
    while (!glfwWindowShouldClose(window)) {

      // Clear framebuffer
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      // Quad transforms (updated each frame)
      vec4 vec = {1.0f, 0.0f, 0.0f, 1.0f};
      mat4 trans = {0};
      glm_mat4_identity(trans); // Create identity matrix
      glm_rotate(trans, glfwGetTime(), (vec3){0.0f, 0.0f, 1.0f}); // Use time as rotation
      glm_scale(trans, (vec3){0.5f, 0.5f, 0.5f});
      gl_obj transform_location = glGetUniformLocation(shader_program, "transform");
      glUniformMatrix4fv(transform_location, 1, GL_FALSE, (const float*)&trans);

      // Draw
      glUseProgram(shader_program);
      glBindTexture(GL_TEXTURE_2D, gl_perlin);
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
