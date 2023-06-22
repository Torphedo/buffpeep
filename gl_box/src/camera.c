#include <cglm/struct.h>
#include <GLFW/glfw3.h>

#include "logging.h"
#include "camera.h"

vec3s camera_pos = {0};
vec3s camera_target = {0};

vec3s camera_orbit_pos = {0};

const float radius = 4.0f;

void update_camera() {
  camera_pos.x = sin(camera_orbit_pos.x) * radius;
  camera_pos.z = cos(camera_orbit_pos.z) * radius;

  // TODO: Vertical orbit camera is clunky
  // This is a little weird/annoying to use, because it will only let you pan
  // in a 90 degree vertical range... Find some way to make this work for at
  // least 180 degrees.
  camera_pos.y = sin(camera_orbit_pos.y) * radius;

  // TODO: Implement flying freecam-style camera [low priority]
}

void camera_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    switch (key) {
      case GLFW_KEY_W:
        camera_orbit_pos.y += 0.1f;
        break;
      case GLFW_KEY_S:
        camera_orbit_pos.y -= 0.1f;
        break;
      case GLFW_KEY_A:
        camera_orbit_pos.x -= 0.1f;
        camera_orbit_pos.z -= 0.1f;
        break;
      case GLFW_KEY_D:
        camera_orbit_pos.x += 0.1f;
        camera_orbit_pos.z += 0.1f;
        break;
    }
  }
}

