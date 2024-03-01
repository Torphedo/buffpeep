#pragma once
#include <stdbool.h>
#include "types.h"

typedef enum {
    DXT1, // BC1
    DXT3, // BC2
    DXT5  // BC3
}img_fmt_compressed;

typedef struct {
    u8* data;
    u16 width; // We should never need an image over 32k...
    u16 height;
    u16 mip_level; // And DEFINITELY not 32k mips...
    u8 bits_per_pixel;
}texture;

// Round image dimensions down to some value
void img_snap(texture* img, u32 size);

u8* image_buf_load(char* filename);
