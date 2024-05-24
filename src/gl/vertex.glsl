#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex_coord;

out vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float ratio;
uniform float tex_ratio;

void main() {
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    gl_Position.x *= ratio;
    gl_Position.y /= ratio;

    tex_coord = a_tex_coord;
    // This seems to help with very wide images, but I don't know if it's
    // mathematically correct or just a convenient coincidence.
    // I used a separate uniform so it can be disabled @ runtime.
    tex_coord.x *= tex_ratio;
    // tex_coord.y *= tex_ratio;
}

