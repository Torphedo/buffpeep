#include <stdbool.h>
#include <stdio.h>

#include <glad/glad.h>
#include <cglm/struct.h>

#include "image.h"
#include "input.h"
#include "logging.h"
#include "types.h"

bool up_last_frame = false;
bool down_last_frame = false;
bool left_last_frame = false;
bool right_last_frame = false;
bool spc_last_frame = false;
bool c_last_frame = false;
bool w_last_frame = false;

void viewer_update(texture* img) {
    bool up = input.k || input.up;
    bool down = input.j || input.down;
    bool left = input.h || input.left;
    bool right = input.l || input.right;
    img->compressed ^= input.c && !c_last_frame; // Toggle if pressed
    bool changed = up ^ up_last_frame | down ^ down_last_frame | left ^ left_last_frame | right ^ right_last_frame | input.space ^ spc_last_frame | input.c ^ c_last_frame | input.w ^ w_last_frame;
    c_last_frame = input.c;

    if (!changed) {
        down_last_frame = down;
        up_last_frame = up;
        left_last_frame = left;
        right_last_frame = right;
        spc_last_frame = input.space;
        return;
    }

    // Increments of 1, or by 4 if compressed (compressed resolution must be a multiple of 4)
    s32 delta_h= ((right * !right_last_frame) - (left * !left_last_frame));
    s32 delta_v = ((up * !up_last_frame) - (down * !down_last_frame));
    u32 multiplier = (1 << img->compressed * 2); // 4 if compressed, 1 if not
    multiplier *= (1 << input.alt * 4); // 16 if held, 1 if not

    img->height += delta_v * multiplier;
    img->width += delta_h * multiplier;

    printf("\033[1F\033[2K"); // Go up a line & clear

    GLint res = (img->height * img->width);
    if (img->compressed) {
        img_snap(img, 4); // Keep image size at multiple of 4
        img->fmt += input.space * !spc_last_frame;
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
            img->unit_size += input.space * !spc_last_frame;
            img->unit_size %= 2;
        }
        else {
            img->channels--;
            img->channels += input.space * !spc_last_frame;
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

    if (input.w && !w_last_frame) {
        img_write(*img);
    }

    down_last_frame = down;
    up_last_frame = up;
    left_last_frame = left;
    right_last_frame = right;
    spc_last_frame = input.space;
    w_last_frame = input.w;
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

mat4s viewer_update_camera(mat4s view) {
    float zoom = 1.0f;
    zoom -= scroll_delta() * 0.05f;

    vec2s pos = glms_vec2_scale(mouse_delta(), 0.001f);
    vec3s offset = (vec3s){pos.x, pos.y, 0.0f};
    view = glms_translate(view, glms_vec3_divs(offset, zoom));

    view = glms_scale(view, (vec3s){1.0f / fabsf(zoom), 1.0f / fabsf(zoom), 1.0f});
    return view;
}
