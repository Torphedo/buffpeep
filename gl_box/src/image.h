#pragma once
#include <stdint.h>

#include "allocator.h"

typedef struct {
  uint8_t* data;
  uint16_t width; // We should never need an image over 32k...
  uint16_t height;
  uint16_t mip_level; // And DEFINITELY not 32k mips...
  uint8_t bits_per_pixel;
}texture;

texture load_dds(allocator_t allocator, char* filename);

