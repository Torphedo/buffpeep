add_executable(gl_box
  "src/main.c"
  "src/shader.c"
  "src/image.c"
  "src/camera.c"
  "src/gl_debug.c"
)
target_link_libraries(gl_box PRIVATE glfw glad)
