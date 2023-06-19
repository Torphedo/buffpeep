#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include "types.h"
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
    u32 size; // Must be 32 (0x20)
    u32 flags;
    u32 format_char_code; // See dwFourCC here: https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dds-pixelformat
    u32 bits_per_pixel;
    u32 red_bitmask;
    u32 green_bitmask;
    u32 blue_bitmask;
    u32 alpha_bitmask;
}dds_pixel_format;

typedef struct dds_header {
    u32 identifier;   // DDS_BEGIN as defined above. aka "file magic" / "magic number".
    u32 size;         // Must be 124 (0x7C)
    u32 flags;
    u32 height;
    u32 width;
    u32 pitch_or_linear_size;
    u32 depth;
    u32 mipmap_count;
    u32 reserved[11]; // Unused
    dds_pixel_format pixel_format;
    u32 caps;         // Flags for complexity of the surface
    u32 caps2;        // Always 0 because we don't use cubemaps or volumes
    u32 caps3;        // Unused
    u32 caps4;        // Unused
    u32 reserved2;    // Unused
}dds_header;

bool has_flag(u32 input, u32 flag) {
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

  u32 flags = header.pixel_format.flags;
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

  u32 texture_size = header.width * header.height * (output.bits_per_pixel / 8);
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

