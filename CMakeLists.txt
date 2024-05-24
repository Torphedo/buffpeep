cmake_minimum_required(VERSION 3.16)
project(buffpeep C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(ext/glad)
add_subdirectory(ext/glfw)

# Internal build tool to generate headers for GLSL shaders
add_executable(txt2h "ext/txt2h.c")

set(GLSL_SOURCES
    src/gl/fragment.glsl
    src/gl/vertex.glsl
)

# Autogenerate string constant headers for each GLSL source file using txt2h.c
set(glsl_headers)
foreach(glsl_src ${GLSL_SOURCES})
    # Replace the ".glsl" with ".h" and put the string in ${file_h}
    get_filename_component(file_h ${glsl_src} NAME_WE)
    set(file_h "${file_h}.h")
    list(APPEND glsl_headers ${file_h}) # Add to list of generated headers

    add_custom_command(
        OUTPUT  ${CMAKE_SOURCE_DIR}/${file_h}
        COMMAND txt2h ${glsl_src}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endforeach()


add_executable(buffpeep
    "src/main.c"
    "src/shader.c"
    "src/file.c"
    "src/image.c"
    "src/gl_setup.c"
    "src/input.c"
    "src/camera.c"
    "src/logging.c"
    "src/viewer.c"
    "src/render_image.c"

    # Adding the GLSL headers here auto-generates them during the build
    ${glsl_headers}
)
target_link_libraries(buffpeep PRIVATE glfw glad)

target_include_directories(buffpeep PRIVATE
  "ext/glfw"
  "ext/glad"
  "ext/cglm/include"
)

file(COPY "src/data/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)

