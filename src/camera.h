#pragma once
#include <cglm/struct.h>

void camera_update(mat4* view);
vec3s camera_facing();
void camera_set_target(vec3s pos);
