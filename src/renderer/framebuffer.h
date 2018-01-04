#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "color3.h"
#include "color4.h"
#include <cstdint>

class framebuffer {
public:
    framebuffer(int w, int h);
    framebuffer() = delete;
    ~framebuffer();

    int pixel_width() const;
    int pixel_height() const;

    float depth_at(int x, int y) const;

    void set_pixel_with_xy_clip(int x, int y, const color3 &c);
    void set_pixel_with_xy_clip(int x, int y, const color4 &c);
    void set_pixel_with_xyz_clip(int x, int y, float z, const color3 &c);
    void set_pixel_with_xyz_clip(int x, int y, float z, const color4 &c);
    void set_pixel_with_z_clip(int x, int y, float z, const color3 &c);
    void set_pixel_with_z_clip(int x, int y, float z, const color4 &c);
    void set_pixel_overwriting_z_buffer(int x, int y, float z, const color3 &c);
    void set_pixel_overwriting_z_buffer(int x, int y, float z, const color4 &c);

    void clear();
    void clear(float gray);
    void prepare_rgba_byte_buffer();
    uint8_t *get_rgba_byte_buffer();

private:
    int width, height;
    color3 *pixels;
    float *depth_buffer;
    std::uint8_t *raw_rgba_buffer;
};

#endif
