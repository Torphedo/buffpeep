add_executable(gl_box
  "main.c"
  "shader.c"
  "image.c"
  "camera.c"
  "gl_debug.c"
  "gl_setup.c"
  "input.c"
  "logging.c"

  "io/file.c"
)
target_link_libraries(gl_box PRIVATE glfw glad)
