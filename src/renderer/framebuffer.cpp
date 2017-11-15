#include "framebuffer.h"
#include "texture.h"
#include "vertex.h"
#include <cmath>
#include <limits>
#include <QtGui/QPainter>

#define OPTIMIZE_FRAMEBUFFER_CLEAR

using namespace std;
using namespace math;

framebuffer::framebuffer(int w, int h) :
    width(w), height(h),
    pixels(new color[width * height]),
    depth_buffer(new float[width * height]) {

    clear();
}

framebuffer::~framebuffer() {
    delete[] pixels;
}

int framebuffer::pixel_width() const {
    return width;
}

int framebuffer::pixel_height() const {
    return height;
}

void framebuffer::set_pixel(int x, int y, const color &c) {
    if (x >= 0 && x < width && y >= 0 && y < height)
        pixels[y * width + x] = c;
}

void framebuffer::set_pixel(int x, int y, float z, const color &c) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        int i = y * width + x;

        if (z < depth_buffer[i]) {
            depth_buffer[i] = z;
            pixels[y * width + x] = c;
        }
    }
}

void framebuffer::set_pixel_unchecked(int x, int y, const color &c) {
    pixels[y * width + x] = c;
}

void framebuffer::set_pixel_unchecked(int x, int y, float z, const color &c) {
    int i = y * width + x;

    if (z < depth_buffer[i]) {
        depth_buffer[i] = z;
        pixels[y * width + x] = c;
    }
}

void framebuffer::clear() {
#ifdef OPTIMIZE_FRAMEBUFFER_CLEAR
    // Kludgy but fast implementation, equivalent to below version
    float *color_data = reinterpret_cast<float*>(pixels);
    
    for (int i = 0, max = sizeof(color) * width * height / sizeof(float); i < max; ++i)
        color_data[i] = 0.0f;

    for (int i = 0, max = width * height; i < max; ++i)
        depth_buffer[i] = std::numeric_limits<float>::infinity();
#else
    // Standard implementation
    for (int i = 0, max = width * height; i < max; ++i) {
        pixels[i] = color();
        depth_buffer[i] = std::numeric_limits<float>::infinity();
    }    
#endif
}

void framebuffer::render_frame_on(QPixmap *buffer) {
    QPainter painter(buffer);

    for (int y = 0, i = 0; y < height; ++y)
        for (int x = 0; x < width; ++x, ++i) {
            color &c = pixels[i];
            painter.setPen(QColor(c.r_to_int(), c.g_to_int(), c.b_to_int()));
            painter.drawPoint(x, y);
        }
}

void framebuffer::render_frame_on(QImage *buffer) {
    for (int y = 0, i = 0; y < height; ++y)
        for (int x = 0; x < width; ++x, ++i)
            buffer->setPixel(x, y, pixels[i].to_ffrgb());
}
