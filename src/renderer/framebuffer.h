#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "color.h"
#include <cstdint>

class framebuffer {
public:
    framebuffer(int w, int h);
    framebuffer() = delete;
    ~framebuffer();

    int pixel_width() const;
    int pixel_height() const;

    void set_pixel(int x, int y, const color &c);
    void set_pixel(int x, int y, float z, const color &c);
    void set_pixel_unchecked(int x, int y, const color &c);
    void set_pixel_unchecked(int x, int y, float z, const color &c);
    void clear();
    uint8_t *get_rgba_byte_buffer();

private:
    int width, height;
    color *pixels;
    float *depth_buffer;
    std::uint8_t *raw_rgba_buffer;
};

#endif
