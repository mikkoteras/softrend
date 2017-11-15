#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "color.h"
#include <cstdint>
#include <QtGui/QPixmap>

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
    void render_frame_on(QPixmap *buffer);
    void render_frame_on(QImage *buffer);

private:
    int width, height;
    color *pixels;
    float *depth_buffer;
};

#endif
