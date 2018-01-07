#ifndef TRIANGLE_RENDER_CONTEXT_H
#define TRIANGLE_RENDER_CONTEXT_H

#include "color3.h"
#include "surface_position.h"
#include "vector.h"
#include <string>

class light_list;

struct triangle_render_context {
public:
    surface_position *vertex[3]; // this enables fast vertex swapping during compute
    surface_position *left_edge_top, *right_edge_top;
    surface_position *left_edge_delta, *right_edge_delta;
    int halftriangle_height;

    math::vector3f eye;
    math::vector3f surface_normal; // only used when not interpolating normals
    math::vector3f surface_midpoint; // used with flat polys
    color3 shade; // used with flat gouraud polys
    const light_list *lights;

    int scanline_divisor; // == number of workers
    int scanline_remainder; // ==> render scanlines where y % scanline_divisor == scanline_remainder

public:
    triangle_render_context();
    ~triangle_render_context();

    triangle_render_context(const triangle_render_context&) = delete;
    triangle_render_context &operator=(const triangle_render_context&) = delete;
    triangle_render_context(triangle_render_context&&) = delete;
    triangle_render_context &operator=(triangle_render_context&&) = delete;

public:
    surface_position &vtx(int i);

    void prepare_edges(); // sort by y

    void prepare_upper_halftriangle();
    void prepare_lower_halftriangle();

    int compute_y_skip(int y) const;
    
private:
    surface_position edge_endpoint[3];
    surface_position long_edge_midpoint;
    surface_position short_edge_delta, long_edge_delta;
};

#endif
