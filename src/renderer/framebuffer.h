#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "color.h"
#include "vector.h"
#include "vertex.h"
#include <cstdint>
#include <QtGui/QPixmap>

class edge;
class texture;
class vertex;

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
    void draw_line(int x1, int y1, const color &c1, int x2, int y2, const color &c2);
    void draw_line(const vertex &v1, const vertex &v2);
    void draw_gouraud_poly(const vertex &v1, const vertex &v2, const vertex &v3);
    void draw_textured_poly(const vertex &v1, const vertex &v2, const vertex &v3,
                            const math::vector2f &uv1, const math::vector2f &uv2, const math::vector2f &uv3,
                            const texture &tex);
    void clear();
    void render_frame_on(QPixmap *buffer);
    void render_frame_on(QImage *buffer);

private:
    static bool polygon_winds_clockwise(const vertex &v1, const vertex &v2, const vertex &v3);
    
private:
    struct edge { // Top-down drawing edge
        edge(const vertex &vx1, const vertex &vx2);
        edge(const vertex &vx1, const vertex &vx2, const math::vector2f &uvx1, const math::vector2f &uvx2);
        int height() const;
        vertex v1, v2;
        math::vector2f uv1, uv2;
    };

    static int find_long_edge(edge *edges);
    void draw_gouraud_halfpoly(const edge &full_edge, const edge &half_edge);
    void draw_textured_halfpoly(const edge &full_edge, const edge &half_edge, const texture &tex);
    
private:
    int width, height;
    color *pixels;
    float *depth_buffer;
};

#endif
