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

void framebuffer::draw_line(int x1, int y1, const color &c1, int x2, int y2, const color &c2) {
    draw_line(vertex(x1, y1, c1), vertex(x2,y2, c2));
}

void framebuffer::draw_line(const vertex &v1, const vertex &v2) {
    float width = fabs(v2.x() - v1.x());
    float height = fabs(v2.y() - v1.y());

    if (width == 0 && height == 0)
        set_pixel(v1.x(), v1.y(), color(v1.c(), v2.c(), 0.5f));
    else if (width >= height) {
        float y, y_delta;
        int x, x_max;
        color c, c_delta;
        
        if (v1.x() < v2.x()) {
            x = v1.x();
            x_max = v2.x();
            y = v1.y();
            y_delta = (v2.y() - v1.y()) / width;
            c = v1.c();
            c_delta = (v2.c() - v1.c()) / width;
        }
        else {
            x = v2.x();
            x_max = v1.x();
            y = v2.y();
            y_delta = (v1.y() - v2.y()) / width;
            c = v2.c();
            c_delta = (v1.c() - v2.c()) / width;

        }

        for (; x <= x_max; ++x) {
            set_pixel(x, y, c);
            y += y_delta;
            c += c_delta;
        }
    }
    else {
        float x, x_delta;
        int y, y_max;
        color c, c_delta;
        
        if (v1.y() < v2.y()) {
            y = v1.y();
            y_max = v2.y();
            x = v1.x();
            x_delta = (v2.x() - v1.x()) / height;
            c = v1.c();
            c_delta = (v2.c() - v1.c()) / height;
        }
        else {
            y = v2.y();
            y_max = v1.y();
            x = v2.x();
            x_delta = (v1.x() - v2.x()) / height;
            c = v2.c();
            c_delta = (v1.c() - v2.c()) / height;
        }
        
        for (; y <= y_max; ++y) {
            set_pixel(x, y, c);
            x += x_delta;
            c += c_delta;
        }
    }
}

void framebuffer::draw_gouraud_poly(const vertex &v1, const vertex &v2, const vertex &v3) {
    edge edges[3] = { edge(v1, v2), edge(v2, v3), edge(v3, v1) };
    int long_edge = find_long_edge(edges);
    int short_edge_1 = (long_edge + 1) % 3;
    int short_edge_2 = (long_edge + 2) % 3;
    draw_gouraud_halfpoly(edges[long_edge], edges[short_edge_1]);
    draw_gouraud_halfpoly(edges[long_edge], edges[short_edge_2]);
}

void framebuffer::draw_textured_poly(const vertex &v1, const vertex &v2, const vertex &v3,
                                     const vector2f &uv1, const vector2f &uv2, const vector2f &uv3,
                                     const texture &tex) {
    if (polygon_winds_clockwise(v1, v2, v3)) {
        edge edges[3] = { edge(v1, v2, uv1, uv2), edge(v2, v3, uv2, uv3), edge(v3, v1, uv3, uv1) };
        int long_edge = find_long_edge(edges);
        int short_edge_1 = (long_edge + 1) % 3;
        int short_edge_2 = (long_edge + 2) % 3;
        draw_textured_halfpoly(edges[long_edge], edges[short_edge_1], tex);
        draw_textured_halfpoly(edges[long_edge], edges[short_edge_2], tex);
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

bool framebuffer::polygon_winds_clockwise(const vertex &v1, const vertex &v2, const vertex &v3) {
    float sum = (v2.x() - v1.x()) * (v2.y() + v1.y()) +
                (v3.x() - v2.x()) * (v3.y() + v2.y()) +
                (v1.x() - v3.x()) * (v1.y() + v3.y());
    return sum > 0.0f;

}

framebuffer::edge::edge(const vertex &vx1, const vertex &vx2) {
    if (vx1.y() < vx2.y()) {
        v1 = vx1;
        v2 = vx2;
    }
    else {
        v1 = vx2;
        v2 = vx1;
    }
}

framebuffer::edge::edge(const vertex &vx1, const vertex &vx2, const vector2f &uvx1, const vector2f &uvx2) {
    if (vx1.y() < vx2.y()) {
        v1 = vx1;
        v2 = vx2;
        uv1 = uvx1;
        uv2 = uvx2;
    }
    else {
        v1 = vx2;
        v2 = vx1;
        uv1 = uvx2;
        uv2 = uvx1;
    }
}

int framebuffer::edge::height() const {
    return v2.y() - v1.y();
}

int framebuffer::find_long_edge(edge *edges) {
    int ranges[3] = {
        edges[0].height(),
        edges[1].height(),
        edges[2].height()
    };

    int r = ranges[0] > ranges[1] ? 0 : 1;
	return ranges[r] > ranges[2] ? r : 2;
}

void framebuffer::draw_gouraud_halfpoly(const edge &long_edge, const edge &short_edge) {
    // long_edge is the one that needs two passes to draw, reaching from top y to bottom y.
    float height_1 = long_edge.height();

    if (height_1 == 0)
        return;
    
    float height_2 = short_edge.height();

    if (height_2 == 0)
        return;

    float y_offset = short_edge.v1.y() - long_edge.v1.y(); // 0 or rows already drawn
    float x1_delta = (long_edge.v2.x() - long_edge.v1.x()) / height_1;
    float x2_delta = (short_edge.v2.x() - short_edge.v1.x()) / height_2;
    float z1_delta = (long_edge.v2.z() - long_edge.v1.z()) / height_1;
    float z2_delta = (short_edge.v2.z() - short_edge.v1.z()) / height_2;
    color c1_delta = (long_edge.v2.c() - long_edge.v1.c()) / height_1;
    color c2_delta = (short_edge.v2.c() - short_edge.v1.c()) / height_2;
    float x1 = long_edge.v1.x() + y_offset * x1_delta;
    float x2 = short_edge.v1.x();
    float z1 = long_edge.v1.z() + y_offset * z1_delta;
    float z2 = short_edge.v1.z();
    color c1 = long_edge.v1.c() + y_offset * c1_delta;
    color c2 = short_edge.v1.c();
    float min_y = short_edge.v1.y();
    float max_y = short_edge.v2.y();
    color c, c_delta;
    float z, z_delta;

    // TODO: handle min_y<0, max_y>buffer
    for (int y = min_y; y < max_y; ++y) {
        // TODO: precompute min and max, don't redo once per line.
        int min_x, max_x;

        if (x1 <= x2) {
            min_x = x1;
            max_x = x2;
            z = z1;
            z_delta = (z2 - z1) / (x2 - x1 + 1);
            c = c1;
            c_delta = (c2 - c1) / (x2 - x1 + 1);
        }
        else {
            min_x = x2;
            max_x = x1;
            z = z2;
            z_delta = (z1 - z2) / (x1 - x2 + 1);
            c = c2;
            c_delta = (c1 - c2) / (x1 - x2 + 1);
        }

        // TODO: handle min_x<0, max_x>buffer
        for (int x = min_x; x <= max_x; ++x, c += c_delta, z += z_delta)
            set_pixel(x, y, z, c);

        x1 += x1_delta;
        x2 += x2_delta;
        z1 += z1_delta;
        z2 += z2_delta;
        c1 += c1_delta;
        c2 += c2_delta;
    }
}

void framebuffer::draw_textured_halfpoly(const edge &long_edge, const edge &short_edge, const texture &tex) {
    // long_edge is the one that needs two passes to draw, reaching from top y to bottom y.
    float height_1 = long_edge.height();

    if (height_1 == 0)
        return;
    
    float height_2 = short_edge.height();

    if (height_2 == 0)
        return;

    float y_offset = short_edge.v1.y() - long_edge.v1.y(); // 0 or rows already drawn
    float x1_delta = (long_edge.v2.x() - long_edge.v1.x()) / height_1;
    float x2_delta = (short_edge.v2.x() - short_edge.v1.x()) / height_2;
    float z1_delta = (long_edge.v2.z() - long_edge.v1.z()) / height_1;
    float z2_delta = (short_edge.v2.z() - short_edge.v1.z()) / height_2;
    float u1_delta = (long_edge.uv2.x() - long_edge.uv1.x()) / height_1;
    float u2_delta = (short_edge.uv2.x() - short_edge.uv1.x()) / height_2;
    float v1_delta = (long_edge.uv2.y() - long_edge.uv1.y()) / height_1;
    float v2_delta = (short_edge.uv2.y() - short_edge.uv1.y()) / height_2;
    float x1 = long_edge.v1.x() + y_offset * x1_delta;
    float x2 = short_edge.v1.x();
    float z1 = long_edge.v1.z() + y_offset * z1_delta;
    float z2 = short_edge.v1.z();
    float u1 = long_edge.uv1.x() + y_offset * u1_delta;
    float u2 = short_edge.uv1.x();
    float v1 = long_edge.uv1.y() + y_offset * v1_delta;
    float v2 = short_edge.uv1.y();
    float min_y = short_edge.v1.y();
    float max_y = short_edge.v2.y();
    float z, z_delta;
    float u, u_delta;
    float v, v_delta;
    
    // TODO: handle min_y<0, max_y>buffer
    for (int y = min_y; y < max_y; ++y) {
        // TODO: precompute min and max, don't redo once per line.
        int min_x, max_x;

        if (x1 <= x2) {
            min_x = x1;
            max_x = x2;
            z = z1;
            z_delta = (z2 - z1) / (x2 - x1 + 1);
            u = u1;
            u_delta = (u2 - u1) / (x2 - x1 + 1);
            v = v1;
            v_delta = (v2 - v1) / (x2 - x1 + 1);
        }
        else {
            min_x = x2;
            max_x = x1;
            z = z2;
            z_delta = (z1 - z2) / (x1 - x2 + 1);
            u = u2;
            u_delta = (u1 - u2) / (x1 - x2 + 1);
            v = v2;
            v_delta = (v1 - v2) / (x1 - x2 + 1);
        }

        // TODO: handle min_x<0, max_x>buffer
        for (int x = min_x; x <= max_x; ++x, z += z_delta, u += u_delta, v += v_delta)
            set_pixel(x, y, z, tex.at(u, v));

        x1 += x1_delta;
        x2 += x2_delta;
        z1 += z1_delta;
        z2 += z2_delta;
        u1 += u1_delta;
        u2 += u2_delta;
        v1 += v1_delta;
        v2 += v2_delta;
    }
}
