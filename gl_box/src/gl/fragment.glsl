#version 330 core
out vec4 fragment_rgba;

in vec3 frag_out;

void main() {
  fragment_rgba = vec4(frag_out, 0);
}
