#include <cglm/struct.h>
#include <GLFW/glfw3.h>

#include "logging.h"
#include "camera.h"
#include "input.h"

vec3s camera_pos = {0};
vec3s camera_target = {0};

vec3s camera_orbit_pos = {0};

const float radius = 4.0f;
const float orbit_speed = 0.05f;

void update_camera() {
  // Update with current input.
  camera_orbit_pos.y += orbit_speed * input.w;
  camera_orbit_pos.y -= orbit_speed * input.s;

  camera_orbit_pos.x -= orbit_speed * input.a;
  camera_orbit_pos.z -= orbit_speed * input.a;

  camera_orbit_pos.x += orbit_speed * input.d;
  camera_orbit_pos.z += orbit_speed * input.d;

  camera_pos.x = sin(camera_orbit_pos.x) * radius;
  camera_pos.z = cos(camera_orbit_pos.z) * radius;

  // TODO: Vertical orbit camera is clunky
  // This is a little weird/annoying to use, because it will only let you pan
  // in a 90 degree vertical range... Find some way to make this work for at
  // least 180 degrees.
  camera_pos.y = sin(camera_orbit_pos.y) * radius;

  // TODO: Implement flying freecam-style camera [low priority]
}

