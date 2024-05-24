#include "input.h"
#include "camera.h"

// Up axis for our camera
vec3s camera_up = {0.0f, 1.0f, 0.0f};

vec3s camera_target = {0};
vec2s camera_orbit_angles = {0};
vec3s camera_pos = {0};

float radius = 4.0f;
const float orbit_speed = 0.025f;
const float move_speed = 0.5f;
const float mouse_sens = 0.015f;

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
vec3s orbit_pos_by_angles(vec2s angles, float orbit_radius) {
    // Get XYZ positions using trig on our angles
    camera_pos = (vec3s){
        .x = sinf(camera_orbit_angles.x),
        .z = cosf(camera_orbit_angles.x),
        .y = sinf(camera_orbit_angles.y)
    };

    // To keep the camera at a constant distance we need to move our horizontal
    // position vector towards (0, 0) by the distance between the radius and the
    // Z position of our Y angle.
    float z_diff = (1.0f - cosf(camera_orbit_angles.y));

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

vec2s get_cursor_delta(vec2s cursor_pos) {
    static vec2s last_cursor = {0};

    // Nullify movement unless click is held
    if (!input.mouse_l) {
        last_cursor = input.cursor;
    }

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
    static vec2s last_scroll = {0};

    const vec2s cursor_delta = get_cursor_delta(input.cursor);

    const vec2s scroll_delta = {
        .x = input.scroll.x - last_scroll.x,
        .y = input.scroll.y - last_scroll.y
    };

    vec3s cam_dir = glms_normalize(camera_facing());
    float forward  = move_speed * (input.w - input.s);
    float side     = move_speed * (input.a - input.d);

    // Nullify vertical movement unless enabled
    float vertical = move_speed * (input.space - input.shift);
    vec3s pos_delta = glms_vec3_scale(cam_dir, forward); // [Camera dir] * forward movement

    // Add [Camera dir rotated by 90 degrees] * side movement to get net movement
    vec3s cam_side = glms_vec3_rotate(cam_dir, glm_rad(90), camera_up);
    cam_side.y = 0;
    pos_delta = glms_vec3_add(pos_delta, glms_vec3_scale(cam_side, side));

    // Use this for "freecam"-style movement
    // pos_delta.y = (cam_dir.y * forward);

    pos_delta.y = vertical;

    // Save state so we can find the delta next time we're called
    last_scroll = input.scroll;

    // Update angles & zoom from mouse input
    camera_orbit_angles = glms_vec2_add(camera_orbit_angles, cursor_delta);
    radius -= scroll_delta.y;
    radius = clampf(radius, 0.05f, 128.0f); // Don't allow <= 0 or really high zoom

    // Update target pos using delta from user input
    camera_target = glms_vec3_add(camera_target, pos_delta);

    // Rendering breaks at exactly 90 and we don't want to be upside-down
    camera_orbit_angles.y = clampf(camera_orbit_angles.y, glm_rad(-89.999f), glm_rad(89.999f));
    
    // Add target position to relative orbit position
    camera_pos = glms_vec3_add(camera_target, orbit_pos_by_angles(camera_orbit_angles, radius));
    
    glm_lookat((float*)&camera_pos, (float*)&camera_target, (float*)&camera_up, *view);
}

vec3s camera_facing() {
    return glms_vec3_sub(camera_target, camera_pos);
}

void camera_set_target(vec3s pos) {
    camera_target = (vec3s){
        .x = pos.x,
        .y = pos.y,
        .z = pos.z,
    };
}

