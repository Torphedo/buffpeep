#pragma once
#include <stdbool.h>
#include "types.h"

typedef enum {
    DXT1, // BC1
    DXT3, // BC2
    DXT5  // BC3
}img_fmt_compressed;

typedef enum {
    A8,
    RGB8,
    RGBA8
}img_fmt_raw;

typedef struct {
    u8* data;
    u16 width; // We should never need an image over 32k...
    u16 height;
    u16 mip_level; // And DEFINITELY not 32k mips...
    u8 bits_per_pixel;
}texture;

texture image_dds_load(char* filename);

u8* image_buf_load(char* filename);

void gl_update_active_tex(texture img, bool compressed, u8 fmt);

