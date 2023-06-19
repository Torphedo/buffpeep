#version 330 core
out vec4 fragment_rgba;

in vec3 frag_out;
in vec2 tex_coord;

uniform sampler2D sampler;

void main() {
  fragment_rgba = texture(sampler, tex_coord);
}
