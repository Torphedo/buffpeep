#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex_coord;

out vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform float ratio;

void main() {
    gl_Position = view * model * vec4(a_pos, 1.0);
    gl_Position.x *= ratio;
    gl_Position.y /= ratio;
    tex_coord = a_tex_coord;
}

