add_executable(buffpeep
  "main.c"
  "shader.c"
  "file.c"
  "image.c"
  "gl_setup.c"
  "input.c"
  "logging.c"
  "viewer.c"
)
target_link_libraries(buffpeep PRIVATE glfw glad)
