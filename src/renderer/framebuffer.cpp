#include "framebuffer.h"
#include "texture.h"
#include <cmath>
#include <limits>

using namespace std;

framebuffer::framebuffer(int w, int h) :
    width(w), height(h),
    pixels(new color3[width * height]),
    depth_buffer(new float[width * height]),
    raw_rgba_buffer(new std::uint8_t[4 * width * height]) {

    clear();
}

framebuffer::~framebuffer() {
    delete[] pixels;
    delete[] depth_buffer;
    delete[] raw_rgba_buffer;
}

int framebuffer::pixel_width() const {
    return width;
}

int framebuffer::pixel_height() const {
    return height;
}

float framebuffer::depth_at(int x, int y) const {
    return depth_buffer[y * width + x];
}

void framebuffer::set_pixel_with_xy_clip(int x, int y, const color3 &c) {
    if (x >= 0 && x < width && y >= 0 && y < height)
        pixels[y * width + x] = c;
}

void framebuffer::set_pixel_with_xy_clip(int x, int y, const color4 &c) {
    if (x >= 0 && x < width && y >= 0 && y < height)
        pixels[y * width + x] += c;
}

void framebuffer::set_pixel_with_xyz_clip(int x, int y, float z, const color3 &c) {
    if (x >= 0 && x < width && y >= 0 && y < height && z <= 0) {
        int i = y * width + x;

        if (z > depth_buffer[i]) {
            depth_buffer[i] = z;
            pixels[i] = c;
        }
    }
}

void framebuffer::set_pixel_with_xyz_clip(int x, int y, float z, const color4 &c) {
    if (x >= 0 && x < width && y >= 0 && y < height && z <= 0) {
        int i = y * width + x;

        if (z > depth_buffer[i]) {
            depth_buffer[i] = z;
            pixels[i] += c;
        }
    }
}

void framebuffer::set_pixel_with_z_clip(int x, int y, float z, const color3 &c) {
    int i = y * width + x;

    if (z <= 0.0f && z > depth_buffer[i]) {
        depth_buffer[i] = z;
        pixels[i] = c;
    }
}

void framebuffer::set_pixel_with_z_clip(int x, int y, float z, const color4 &c) {
    int i = y * width + x;

    if (z <= 0.0f && z > depth_buffer[i]) {
        depth_buffer[i] = z;
        pixels[i] += c;
    }
}

void framebuffer::set_pixel_overwriting_z_buffer(int x, int y, float z, const color3 &c) {
    int i = y * width + x;

    if (z <= 0.0f) {
        depth_buffer[i] = z;
        pixels[i] = c;
    }
}

void framebuffer::set_pixel_overwriting_z_buffer(int x, int y, float z, const color4 &c) {
    int i = y * width + x;

    if (z <= 0.0f) {
        depth_buffer[i] = z;
        pixels[i] += c;
    }
}

void framebuffer::clear() {
#ifdef OPTIMIZE_FRAMEBUFFER_CLEAR
    float *color_data = reinterpret_cast<float*>(pixels);

    for (int i = 0, max = sizeof(color3) * width * height / sizeof(float); i < max; ++i)
        color_data[i] = 0.0f;

    for (int i = 0, max = width * height; i < max; ++i)
        depth_buffer[i] = -std::numeric_limits<float>::infinity();
#else
    for (int i = 0, max = width * height; i < max; ++i) {
        pixels[i] = color();
        depth_buffer[i] = -std::numeric_limits<float>::infinity();
    }
#endif
}

void framebuffer::clear(float gray) {
#ifdef OPTIMIZE_FRAMEBUFFER_CLEAR
    float *color_data = reinterpret_cast<float*>(pixels);

    for (int i = 0, max = width * height * 3; i < max;) {
        color_data[i++] = gray;
        color_data[i++] = gray;
        color_data[i++] = gray;
    }

    for (int i = 0, max = width * height; i < max; ++i)
        depth_buffer[i] = -std::numeric_limits<float>::infinity();
#else
    for (int i = 0, max = width * height; i < max; ++i) {
        pixels[i] = color();
        depth_buffer[i] = -std::numeric_limits<float>::infinity();
    }
#endif
}

void framebuffer::prepare_rgba_byte_buffer() {
    float *source = reinterpret_cast<float*>(pixels); // TODO: not quite evil, but slightly naughty.
    std::uint8_t *destination = raw_rgba_buffer;
    
    for (int i = 0, max = width * height; i < max; ++i) {
        *destination++ = static_cast<uint8_t>(255.0 * *source++);
        *destination++ = static_cast<uint8_t>(255.0 * *source++);
        *destination++ = static_cast<uint8_t>(255.0 * *source++);
        *destination++ = 0xFF;
    }
}

uint8_t *framebuffer::get_rgba_byte_buffer() {
    return raw_rgba_buffer;
}
