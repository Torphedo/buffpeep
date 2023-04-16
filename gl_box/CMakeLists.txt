add_executable(gl_box
    "src/main.c"
    "src/shader.c"
)
target_link_libraries(gl_box PRIVATE glfw glad)
