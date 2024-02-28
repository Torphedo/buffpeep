#include <stdbool.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GL/glext.h>

#include "image.h"
#include "input.h"
#include "logging.h"
#include "types.h"

bool compressed = true;
u8 img_fmt = DXT5;

// For raw formats
u8 unit_size = 0; // 0 = u8, 1 = u16, 2 = u32
u8 channels = 4;
bool up_last_frame = false;
bool down_last_frame = false;
bool left_last_frame = false;
bool right_last_frame = false;
bool spc_last_frame = false;

void viewer_update(texture img) {
    bool up = input.k || input.up;
    bool down = input.j || input.down;
    compressed ^= input.space && !spc_last_frame; // Toggle if space is pressed
    bool changed = up ^ up_last_frame | down ^ down_last_frame | input.left ^ left_last_frame | input.right ^ right_last_frame | input.space ^ spc_last_frame;
    spc_last_frame = input.space;


    if (!changed) {
        down_last_frame = down;
        up_last_frame = up;
        left_last_frame = input.left;
        right_last_frame = input.right;
        return;
    }

    printf("\033[1F\033[2K"); // Go up a line & clear

    GLint res = (img.height * img.width);
    if (compressed) {
        img_fmt += up * !up_last_frame;
        img_fmt -= down * !down_last_frame;
        img_fmt %= 3;

        GLenum format = 0;
        GLint size = res;
        switch (img_fmt) {
            case DXT3:
                LOG_MSG(info, "DXT3");
                format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                break;
            case DXT5:
                LOG_MSG(info, "DXT5");
                format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                break;
            default:
                LOG_MSG(info, "DXT1");
                format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                size /= 2;
                break;
        };
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, img.width, img.height, 0, size, img.data);
    }
    else {
        unit_size += up * !up_last_frame;
        unit_size -= down * !down_last_frame;
        unit_size %= 2;

        channels += input.right * !right_last_frame;
        channels -= input.left * !left_last_frame;
        channels %= 4;
        LOG_MSG(info, "%d-bit, %d channels", (1 << unit_size) * 8, channels + 1);
        GLenum gl_size = GL_UNSIGNED_BYTE + (unit_size * 2);
        GLint format = GL_RGBA;
        switch (channels + 1) {
            case 1:
                format = GL_RED;
                break;
            case 2:
                format = GL_RG;
                break;
            case 3:
                format = GL_RGB;
                break;
            default:
                format = GL_RGBA;
                break;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, format, img.width, img.height, 0, format, gl_size, img.data);
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    printf("\n");

    down_last_frame = down;
    up_last_frame = up;
    left_last_frame = input.left;
    right_last_frame = input.right;
}
