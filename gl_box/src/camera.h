#pragma once
#include <GLFW/glfw3.h>
#include <cglm/struct.h>

extern vec3s camera_pos;
extern vec3s camera_target;
static vec3 camera_up = {0.0f, 1.0f, 0.0f};

void update_camera();
void camera_key_callback(GLFWwindow* window, int key, int scancode, int actions, int mods);

