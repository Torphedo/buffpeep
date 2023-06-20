#include "camera.h"
#include "GLFW/glfw3.h"

vec3s camera_pos = {0};
vec3s camera_target = {0};
vec3 camera_up = {0.0f, 1.0f, 0.0f};

void update_camera() {
  camera_target = camera_pos;
  camera_target.x += 0.1f;
}

// If we want the camera movement to always be relative to the view (e.g. for a
// freecam-style flying controller), then we'll need a "camera front" offset
// for the camera to look at, then move along the local axes.
//
// But since we'll probably need a third-person orbiting camera, we need to add
// a normalized camera position vector to the camera target, then just look at
// the target. To rotate, we just add to the position vector and normalize it.
void camera_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    switch (key) {
      case GLFW_KEY_W:
        camera_pos.x += 0.2f;
        break;
      case GLFW_KEY_S:
        camera_pos.x -= 0.2f;
        break;
      case GLFW_KEY_A:
        camera_pos.z -= 0.2f;
        break;
      case GLFW_KEY_D:
        camera_pos.z += 0.2f;
        break;
    }
  }
}

