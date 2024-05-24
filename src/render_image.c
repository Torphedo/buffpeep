#include <stdlib.h>
#include <stddef.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "logging.h"
#include "shader.h"
#include "viewer.h"
#include "camera.h"
#include "render_image.h"

vertex quad_vertices[] = {
    { .position = {0.5f, 0.5f, 0.0f},
      .tex_coord = {1.0f, 1.0f}
    },
    {
      .position = {0.5f, -0.5f, 0.0f},
      .tex_coord = {1.0f, 0.0f}
    },
    {
      .position = {-0.5f, -0.5f, 0.0f},
      .tex_coord = {0.0f, 0.0f}
    },
    {
      .position = {-0.5f,  0.5f, 0.0f},
      .tex_coord = {0.0f, 1.0f}
    },
    { .position = {0.5f, 0.5f, 0.0f},
      .tex_coord = {1.0f, 1.0f}
    },
    {
      .position = {-0.5f, -0.5f, 0.0f},
      .tex_coord = {0.0f, 0.0f}
    }
};

static const char frag[] = {
    #include "gl/fragment.h"
};

static const char vert[] = {
    #include "gl/vertex.h"
};

typedef struct {
    gl_obj vertex_array;
    gl_obj vertex_buffer;
    gl_obj shader_program;
    gl_obj gl_img;
    texture* img;

    gl_obj u_ratio;
    gl_obj u_proj;
    gl_obj u_model;
    gl_obj u_view;
}img_state;

void* image_init(texture* img) {
    img_state* state = calloc(1, sizeof(*state));
    if (state == NULL) {
        LOG_MSG(error, "Couldn't allocate rendering context!\n");
        return NULL;
    }
    state->img = img;

    // Setup VAO to store our state
    state->vertex_array = 0;
    glGenVertexArrays(1, &state->vertex_array);
    glBindVertexArray(state->vertex_array);
    
    // Setup vertex buffer
    state->vertex_buffer = 0;
    glGenBuffers(1, &state->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, state->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    
    // Create vertex layout
    glVertexAttribPointer(0, sizeof(vec3f) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, sizeof(vec2f) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, tex_coord));
    glEnableVertexAttribArray(1);
    
    // Load and compile shaders
    gl_obj vertex_shader = shader_compile_src(vert, GL_VERTEX_SHADER);
    gl_obj fragment_shader = shader_compile_src(frag, GL_FRAGMENT_SHADER);
    
    if (vertex_shader == 0 || fragment_shader == 0) {
        // All needed error information should already be given by the shader
        // compiler function and our debug callbacks.
        LOG_MSG(error, "failed to compile shaders.\n");
        return NULL;
    }

    // Link the compiled shaders
    state->shader_program = glCreateProgram();
    glAttachShader(state->shader_program, vertex_shader);
    glAttachShader(state->shader_program, fragment_shader);
    glLinkProgram(state->shader_program);

    // Make sure linking succeeded
    if (!shader_link_check(state->shader_program)) {
        // No need to print, link check prints messages on failure.
        return NULL;
    }
    glUseProgram(state->shader_program);

    state->u_ratio = glGetUniformLocation(state->shader_program, "ratio");
    state->u_proj = glGetUniformLocation(state->shader_program, "projection");
    state->u_model = glGetUniformLocation(state->shader_program, "model");
    state->u_view = glGetUniformLocation(state->shader_program, "view");


    // Load texture
    state->gl_img = 0;
    glGenTextures(1, &state->gl_img);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state->gl_img);

    // Wrapping & filtering settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, img->width, img->height, 0, (img->width * img->height) / 2, img->data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glEnable(GL_DEPTH_TEST);
    // Enable transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Delete the individual shader objects
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Unbind our buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return state;
}

void image_render(void* ctx) {
    img_state* state = (img_state*)ctx;

    // Manages active texture's format, dimensions, etc.
    viewer_update(state->shader_program, state->img);

    // Quad transforms (updated each frame)
    float ratio = (float)state->img->width / (float)state->img->height;
    glUniform1f(state->u_ratio, ratio);

    // Upload projection matrix
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    mat4 projection = {0};
    glm_perspective_rh_no(glm_rad(45), (float)mode->width / (float)mode->height, 0.1f, 1000.0f, projection);
    glUniformMatrix4fv(state->u_proj, 1, GL_FALSE, (const float*)&projection);

    mat4s model = glms_mat4_identity();
    model = glms_rotate(model, glm_rad(180.0f), (vec3s){1.0f, 0.0f, 0.0f});
    // model = glms_scale(model, (vec3s){(float)cur_height / (float)(cur_width), 1.0f, 1.0f}); // half screen

    glUniformMatrix4fv(state->u_model, 1, GL_FALSE, (const float*)&model.raw);

    mat4 view = {0};
    glm_mat4_identity(view);
    camera_update(&view);
    glUniformMatrix4fv(state->u_view, 1, GL_FALSE, (const float*)view);

    // Draw
    glUseProgram(state->shader_program);
    glBindVertexArray(state->vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(quad_vertices) / sizeof(*quad_vertices));
}

void image_destroy(void* ctx) {
    img_state* state = (img_state*)ctx;

    glDeleteProgram(state->shader_program);
    glDeleteVertexArrays(1, &state->vertex_array);
    glDeleteBuffers(1, &state->vertex_buffer);
}

renderer image_renderer = {
    .init = image_init,
    .render = image_render,
    .destroy = image_destroy,
};

