#ifndef VECTOR_H
#define VECTOR_H
/// @file vector.h
/// @brief Integer and floating-point vector types

#include <common/int.h>
#include <cglm/struct.h>

/// Byteswap a 3D vector
void vec3_byteswap(vec3* v);

/// 3D integer vector (signed 8-bit)
typedef union {
    struct {
        s8 x;
        s8 y; // Height
        s8 z;
    };
    s8 raw[3];
}vec3s8;

/// Compare 2 8-bit 3D integer vectors
bool vec3s8_eq(vec3s8 a, vec3s8 b);

/// Convert an 8-bit 3D vector to a floating point one
vec3s vec3_from_vec3s8(vec3s8 vu8, float scale);

/// 3D integer vector (signed 16-bit)
typedef union {
    struct {
        s16 x;
        s16 y; // Height
        s16 z;
    };
    s16 raw[3];
}vec3s16;

/// Convert an 8-bit 3D vector to a 16-bit one
// TODO: These are both signed, why is is it named "vec3u8"?
bool vec3u8_eq_vec3s16(vec3s8 a, vec3s16 b);

/// Compare 2 16-bit 3D integer vectors
bool vec3s16_eq(vec3s16 a, vec3s16 b);

/// Convert a 16-bit 3D vector to a floating point one
vec3s vec3_from_vec3s16(vec3s16 vs16, float scale);

/// 8-bit RGBA color
typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
}rgba8;

/// Convert 8-bit color to 4D floating point vector
vec4s vec4_from_rgba8(rgba8 c);

/// Linear interpolation
#define LERP(a, b, t) ((a * (1 - t)) + (b * t))

#endif // VECTOR_H
