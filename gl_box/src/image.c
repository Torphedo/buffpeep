#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include "image.h"
#include "logging.h"

typedef enum dds_format_flags {
    DDPF_ALPHAPIXELS = 0x00000001,
    DDPF_ALPHA       = 0x00000002,
    DDPF_FOURCC      = 0x00000004,
    DDPF_RGB         = 0x00000040,
    DDPF_YUV         = 0x00000200,
    DDPF_LUMINANCE   = 0x00020000
}dds_format_flags;

typedef struct dds_pixel_format {
    uint32_t size; // Must be 32 (0x20)
    uint32_t flags;
    uint32_t format_char_code; // See dwFourCC here: https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dds-pixelformat
    uint32_t bits_per_pixel;
    uint32_t red_bitmask;
    uint32_t green_bitmask;
    uint32_t blue_bitmask;
    uint32_t alpha_bitmask;
}dds_pixel_format;

typedef struct dds_header {
    uint32_t identifier;   // DDS_BEGIN as defined above. aka "file magic" / "magic number".
    uint32_t size;         // Must be 124 (0x7C)
    uint32_t flags;
    uint32_t height;
    uint32_t width;
    uint32_t pitch_or_linear_size;
    uint32_t depth;
    uint32_t mipmap_count;
    uint32_t reserved[11]; // Unused
    dds_pixel_format pixel_format;
    uint32_t caps;         // Flags for complexity of the surface
    uint32_t caps2;        // Always 0 because we don't use cubemaps or volumes
    uint32_t caps3;        // Unused
    uint32_t caps4;        // Unused
    uint32_t reserved2;    // Unused
}dds_header;

bool has_flag(uint32_t input, uint32_t flag) {
  return (input ^ flag) != input;
}

texture load_dds(allocator_t allocator, char* filename) {
  texture output = {0};
  static dds_header header = {0};
  FILE* file = fopen(filename, "rb");
  if (file == NULL) {
    LOG_MSG(error, "failed to open %s\n", filename);
    return output;
  }
  LOG_MSG(info, "loading %s...\n", filename);

  fread(&header, sizeof(header), 1, file);

  uint32_t flags = header.pixel_format.flags;
  if (has_flag(flags, DDPF_RGB) || has_flag(flags, DDPF_LUMINANCE) || has_flag(flags, DDPF_YUV) || has_flag(flags, DDPF_ALPHA)) {
    output.bits_per_pixel = header.pixel_format.bits_per_pixel;
  }
  else if (has_flag(flags, DDPF_ALPHAPIXELS)) {
    output.bits_per_pixel = 32;
  }
  else if (has_flag(flags, DDPF_FOURCC)) {
    // Ignore char code of "DX10" indicating more advanced formats like BC7, for now
    output.bits_per_pixel = 16;
  }

  uint32_t texture_size = header.width * header.height * (output.bits_per_pixel / 8);
  output.data = allocator.calloc(1, texture_size);
  if (output.data == NULL) {
    fclose(file);
    return output;
  }

  fread(output.data, texture_size, 1, file);
  fclose(file);

  output.height = header.height;
  output.width = header.width;
  output.mip_level = header.mipmap_count;

  LOG_MSG(info, "%d bits per pixel\n", output.bits_per_pixel);
  LOG_MSG(info, "%dx%d\n", output.width, output.height);
  return output;
}

