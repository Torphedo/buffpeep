#include <cglm/struct.h>

#include "input.h"

// Up axis for our camera
static vec3 camera_up = {0.0f, 1.0f, 0.0f};

static vec3s camera_target = {0};
static vec2s camera_orbit_angles = {0};

static const float radius = 4.0f;
static const float orbit_speed = 0.025f;

float clampf(float x, float min, float max) {
    if (x > max) {
        return max;
    }
    else if (x < min) {
        return min;
    }
    else {
        return x;
    }
}

// Lengthen/shorten a vector by an arbitrary amount.
vec2s vec2_addmag(vec2s v, float amount) {
  float magnitude = glms_vec2_norm(v);
  // Turn our amount into a scalar we can multiply the vector by
  float scalar = 1.0f - (amount / magnitude);

  // Scale the vector.
  v = glms_vec2_scale(v, scalar);
  return v;
}

void update_camera(mat4* view) {
  vec3s camera_pos = {0};
  // Update with current input.
  camera_orbit_angles.y += orbit_speed * input.w;
  camera_orbit_angles.y -= orbit_speed * input.s;
  camera_orbit_angles.x -= orbit_speed * input.a;
  camera_orbit_angles.x += orbit_speed * input.d;

  // Rendering breaks at exactly 90 and we don't want to be upside-down
  camera_orbit_angles.y = clampf(camera_orbit_angles.y, glm_rad(-89.9999f), glm_rad(89.9999f));

  // Get XYZ positions using trig on our angles
  camera_pos.x = sin(camera_orbit_angles.x);
  camera_pos.z = cos(camera_orbit_angles.x);
  camera_pos.y = sin(camera_orbit_angles.y);

  // To keep the camera at a constant distance we need to move our horizontal
  // position vector towards (0, 0) by the distance between the radius and the
  // Z position of our Y angle.
  float z_diff = (1.0f - cos(camera_orbit_angles.y));

  // Make sure we're SHORTENING the vector, not adding to it.
  z_diff = fabsf(z_diff); 

  // Shorten our horizontal vector by the calculated amount.
  vec2s horizontal_pos = {camera_pos.x, camera_pos.z};
  horizontal_pos = vec2_addmag(horizontal_pos, z_diff);

  camera_pos.x = horizontal_pos.x;
  camera_pos.z = horizontal_pos.y;

  // Scale our position to the orbit radius
  camera_pos = glms_vec3_scale_as(camera_pos, radius);

  // TODO: Implement flying freecam-style camera [low priority]
  glm_lookat((float*)&camera_pos, (float*)&camera_target, camera_up, *view);
}

