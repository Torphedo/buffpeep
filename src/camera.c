#include <cglm/struct.h>

#include "input.h"
#include "logging.h"

// Up axis for our camera
static vec3 camera_up = {0.0f, 1.0f, 0.0f};

static vec3s camera_target = {0};
static vec2s camera_orbit_angles = {0};

static float radius = 4.0f;
static const float orbit_speed = 0.025f;
static const float mouse_sens = 0.015f;

bool invert_mouse_x = true;
bool invert_mouse_y = false;

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

// Get the camera position relative to an orbit center-point based on the
// rotation angle
vec3s orbit_pos_by_angles(vec2s angles, u32 orbit_radius) {
    // Get XYZ positions using trig on our angles
    vec3s camera_pos = {
        .x = sin(camera_orbit_angles.x),
        .z = cos(camera_orbit_angles.x),
        .y = sin(camera_orbit_angles.y)
    };

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
    camera_pos = glms_vec3_scale_as(camera_pos, orbit_radius);

    return camera_pos;
}

vec2s get_cursor_delta(vec2f cursor_pos) {
    static vec2f last_cursor = {0};
    vec2s cursor_delta = {
        .x = (cursor_pos.x - last_cursor.x) * mouse_sens,
        .y = (cursor_pos.y - last_cursor.y) * mouse_sens
    };

    // Save state so we can find the delta next time we're called
    last_cursor = cursor_pos;

    // Invert sign as needed.
    if (invert_mouse_x) {
        cursor_delta.x = -cursor_delta.x;
    }
    if (invert_mouse_y) {
        cursor_delta.y = -cursor_delta.y;
    }

    return cursor_delta;
}

void camera_update(mat4* view) {
    vec3s camera_pos = {0};
    static vec2f last_cursor = {0};
    static vec2f last_scroll = {0};

    const vec2s cursor_delta = get_cursor_delta(input.cursor);

    const vec2f scroll_delta = {
        .x = input.scroll.x - last_scroll.x,
        .y = input.scroll.y - last_scroll.y
    };
    const vec3s pos_delta = {
        .x = (input.w - input.s) * orbit_speed,
        .y = 0.0f,
        .z = (input.d - input.a) * orbit_speed
    };

    // Save state so we can find the delta next time we're called
    last_scroll = input.scroll;

    // Update angles & zoom from mouse input
    camera_orbit_angles = glms_vec2_add(camera_orbit_angles, cursor_delta);
    radius -= scroll_delta.y;

    // Update with keyboard input.
    camera_target = glms_vec3_add(camera_target, pos_delta);

    // Rendering breaks at exactly 90 and we don't want to be upside-down
    camera_orbit_angles.y = clampf(camera_orbit_angles.y, glm_rad(-89.999f), glm_rad(89.999f));
    
    // Add target position to relative orbit position
    camera_pos = glms_vec3_add(camera_target, orbit_pos_by_angles(camera_orbit_angles, radius));
    
    // TODO: Implement flying freecam-style camera [low priority]
    glm_lookat((float*)&camera_pos, (float*)&camera_target, camera_up, *view);
}

