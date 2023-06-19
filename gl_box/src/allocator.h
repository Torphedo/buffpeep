#pragma once
#include <stdint.h>
#include <stddef.h>
#include <malloc.h>

// Normally we shouldn't use the _t, but this lets functions refer to their
// allocators just as "allocator", which is more convenient.
typedef struct {
  void* (*malloc)(size_t size);
  void* (*calloc)(size_t num, size_t size);
  void (*free)(void* ptr);
}allocator_t;

static const allocator_t allocator_default = {
  .malloc = malloc,
  .calloc = calloc,
  .free = free
};

