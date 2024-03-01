#include <stdio.h>
#include <stdbool.h>

#include "types.h"
#include "image.h"
#include "file.h"

// 32MiB image data buffer for OpenGL to copy from.
u8 img_buf[32 * 0x400 * 0x400] = {0};

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

void img_snap(texture* img, u32 size) {
    if (img->width % size) {
        img->width -= (img->width % size);
    }
    if (img->height % size) {
        img->height -= (img->height % size);
    }
}

bool has_flag(u32 input, u32 flag) {
    return (input ^ flag) != input;
}

}

u8* image_buf_load(char* filename) {
    // TODO: If we allow loading a new texture at runtime, memset buffer to 0.
    file_load_existing(filename, (u8*)&img_buf, sizeof(img_buf));

    return (u8*)&img_buf;
}
