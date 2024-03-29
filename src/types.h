#pragma once
#include <stdint.h>

typedef uint64_t u64;
typedef int64_t s64;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint8_t u8;
typedef int8_t s8;

typedef s32 gl_obj;

typedef struct {
    float x;
    float y;
}vec2f;

typedef struct {
    float x;
    float y;
    float z;
}vec3f;

typedef struct {
    float x;
    float y;
    float z;
    float w;
}vec4f;

typedef struct {
    vec3f position;
    vec2f tex_coord;
}vertex;

typedef struct {
    vertex* vertices;
    u32 index_count;
    u32* indices;
}mesh;

