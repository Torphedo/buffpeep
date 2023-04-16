#version 330 core
out vec4 fragment_rgba;

in vec4 frag_out;

void main() {
    fragment_rgba = frag_out;
}