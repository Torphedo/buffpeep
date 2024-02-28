add_executable(gl_box
  "main.c"
  "shader.c"
  "file.c"
  "image.c"
  "gl_setup.c"
  "input.c"
  "logging.c"
  "viewer.c"
)
target_link_libraries(gl_box PRIVATE glfw glad)
