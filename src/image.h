#pragma once
#include <stdint.h>

#include "types.h"
#include "allocator.h"

typedef struct {
  u8* data;
  u16 width; // We should never need an image over 32k...
  u16 height;
  u16 mip_level; // And DEFINITELY not 32k mips...
  u8 bits_per_pixel;
}texture;

texture load_dds(allocator_t allocator, char* filename);

