#ifndef TRIANGLE_RENDER_CONTEXT_H
#define TRIANGLE_RENDER_CONTEXT_H

#include "color3.h"
#include "surface_position.h"
#include "math/vector.h"
#include <string>

struct triangle_render_context {
public:
    surface_position *vertex[3]; // this enables fast vertex swapping during compute
    surface_position *left_edge_top[2], *right_edge_top[2];
    surface_position *left_edge_delta[2], *right_edge_delta[2];
    int halftriangle_height[2];

    math::vector3f surface_normal; // only used when not interpolating normals
    math::vector3f surface_midpoint; // used with flat polys
    color3 shade; // used with flat gouraud polys

public:
    triangle_render_context();
    ~triangle_render_context();

    triangle_render_context(const triangle_render_context&) = delete;
    triangle_render_context &operator=(const triangle_render_context&) = delete;
    triangle_render_context(triangle_render_context&&) = delete;
    triangle_render_context &operator=(triangle_render_context&&) = delete;

public:
    surface_position &vtx(int i);
    const surface_position &vtx(int i) const;

    void prepare_edges(); // sort by y

    void prepare_upper_halftriangle();
    void prepare_lower_halftriangle();

    int compute_y_skip(int y, size_t thread_index, size_t num_threads) const;
    
private:
    surface_position edge_endpoint[3];
    surface_position long_edge_midpoint;
    surface_position short_edge_delta[2], long_edge_delta;
};

#endif
