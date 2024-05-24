#include <stdbool.h>
#include <stdio.h>

#include <glad/glad.h>
#include <cglm/struct.h>

#include "image.h"
#include "camera.h"
#include "input.h"
#include "logging.h"
#include "types.h"

input_internal input_prev = {
    .space = true, // Update texture state on startup
};

bool use_texcoord_hack = true;

void viewer_update(gl_obj shader, texture* img) {
    bool up = (input.k && !input_prev.k) || (input.up && !input_prev.up);
    bool down = (input.j && !input_prev.j) || (input.down && input_prev.down);
    bool left = (input.h && !input_prev.h) || (input.left && !input_prev.left);
    bool right = (input.l && !input_prev.l) || (input.right && !input_prev.right);
    bool space = (input.space * !input_prev.space);
    img->compressed ^= input.c && !input_prev.c; // Toggle if pressed
    use_texcoord_hack ^= (input.t && !input_prev.t);

    // 
    gl_obj u_tex_ratio = glGetUniformLocation(shader, "tex_ratio");
    float ratio = (float)img->width / (float)img->height;
    if (!use_texcoord_hack) {
        ratio = 1.0f;
    }
    glUniform1f(u_tex_ratio, ratio);

    // Increments of 1, or by 4 if compressed (compressed resolution must be a multiple of 4)
    s32 delta_h = right - left;
    s32 delta_v = up - down;
    u32 multiplier = (1 << img->compressed * 2); // 4 if compressed, 1 if not
    multiplier *= (1 << input.alt * 4); // 16 if held, 1 if not

    img->height += delta_v * multiplier;
    img->width  += delta_h * multiplier;

    printf("\033[1F\033[2K"); // Go up a line & clear

    GLint res = (img->height * img->width);
    if (img->compressed) {
        img_snap(img, 4); // Keep image size at multiple of 4
        img->fmt += space;
        img->fmt %= 3;

        GLenum format = 0;
        GLint size = res;
        switch (img->fmt) {
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
        printf(" %dx%d\n", img->width, img->height);
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, img->width, img->height, 0, size, img->data);
    }
    else {
        if (input.shift) {
            img->unit_size += space;
            img->unit_size %= 2;
        }
        else {
            img->channels--;
            img->channels += space;
            img->channels = (img->channels % 4) + 1;
        }

        LOG_MSG(info, "%d-bit, %d channels", (1 << img->unit_size) * 8, img->channels);
        printf(" %dx%d\n", img->width, img->height);
        GLenum gl_size = GL_UNSIGNED_BYTE + (img->unit_size * 2);
        GLint format;
        switch (img->channels) {
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

        glTexImage2D(GL_TEXTURE_2D, 0, format, img->width, img->height, 0, format, gl_size, img->data);
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    if (input.w && !input_prev.w) {
        img_write(*img);
    }

    input_prev = input;
}

vec2s mouse_delta() {
    static vec2s prev_mouse = {0};
    if (!input.mouse_l) {
        prev_mouse.x = 0;
        prev_mouse.y = 0;
        return (vec2s){0};
    }
    if (input.mouse_l && prev_mouse.x == 0 && prev_mouse.y == 0) {
        prev_mouse.x = input.cursor.x;
        prev_mouse.y = input.cursor.y;
    }

    vec2s output = {
            .x = (input.cursor.x - prev_mouse.x),
            .y = -(input.cursor.y - prev_mouse.y)
    };
    prev_mouse.x = input.cursor.x;
    prev_mouse.y = input.cursor.y;

    return output;
}

float scroll_delta() {
    static float prev_scroll = 0.0f;
    float output = input.scroll.y - prev_scroll;

    prev_scroll = input.scroll.y;

    return output;
}

