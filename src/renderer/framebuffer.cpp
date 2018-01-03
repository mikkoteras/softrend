#include "framebuffer.h"
#include "texture.h"
#include <cmath>
#include <limits>

#define OPTIMIZE_FRAMEBUFFER_CLEAR

using namespace std;

framebuffer::framebuffer(int w, int h) :
    width(w), height(h),
    pixels(new color[width * height]),
    depth_buffer(new float[width * height]),
    raw_rgba_buffer(new uint8_t[4 * width * height]) {

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

void framebuffer::set_pixel(int x, int y, const color &c) {
    if (x >= 0 && x < width && y >= 0 && y < height)
        pixels[y * width + x].superimpose(c);
}

void framebuffer::set_pixel(int x, int y, float z, const color &c) {
    if (x >= 0 && x < width && y >= 0 && y < height && z <= 0) {
        int i = y * width + x;

        if (z > depth_buffer[i]) {
            depth_buffer[i] = z;
            pixels[i].superimpose(c);
        }
    }
}

float framebuffer::depth_at(int x, int y) const {
    return depth_buffer[y * width + x];
}

void framebuffer::set_pixel_unchecked(int x, int y, float z, const color &c) {
    int i = y * width + x;

    if (z <= 0.0f && z > depth_buffer[i]) {
        depth_buffer[i] = z;
        pixels[i].superimpose(c);
    }
}

void framebuffer::clear() {
#ifdef OPTIMIZE_FRAMEBUFFER_CLEAR
    // Kludgy but fast implementation, equivalent to below version
    float *color_data = reinterpret_cast<float*>(pixels);

    for (int i = 0, max = sizeof(color) * width * height / sizeof(float); i < max; ++i)
        color_data[i] = 0.0f;

    for (int i = 0, max = width * height; i < max; ++i)
        depth_buffer[i] = -std::numeric_limits<float>::infinity();
#else
    // Standard implementation
    for (int i = 0, max = width * height; i < max; ++i) {
        pixels[i] = color();
        depth_buffer[i] = -std::numeric_limits<float>::infinity();
    }
#endif
}

void framebuffer::clear(float gray) {
#ifdef OPTIMIZE_FRAMEBUFFER_CLEAR
    // Kludgy but fast implementation, equivalent to below version
    float *color_data = reinterpret_cast<float*>(pixels);

    for (int i = 0, max = width * height * 4; i < max;) {
        color_data[i++] = gray;
        color_data[i++] = gray;
        color_data[i++] = gray;
        color_data[i++] = 0.0f;
    }

    for (int i = 0, max = width * height; i < max; ++i)
        depth_buffer[i] = -std::numeric_limits<float>::infinity();
#else
    // Standard implementation
    for (int i = 0, max = width * height; i < max; ++i) {
        pixels[i] = color();
        depth_buffer[i] = -std::numeric_limits<float>::infinity();
    }
#endif
}

void framebuffer::prepare_rgba_byte_buffer() {
    float *float_buffer = reinterpret_cast<float*>(pixels); // TODO: not quite evil, but slightly naughty.

    for (int i = 0, max = 4 * width * height; i < max; ++i)
        raw_rgba_buffer[i] = static_cast<uint8_t>(255.0 * float_buffer[i]);
}

uint8_t *framebuffer::get_rgba_byte_buffer() {
    return raw_rgba_buffer;
}
