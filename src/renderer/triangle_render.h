#ifndef TRIANGLE_RENDER_H
#define TRIANGLE_RENDER_H

#include "vector.h"

// All the data required for a triangle render. Depending on the shading
// model and available data, many of the fields might not get used.
struct vertex_data {
    math::vector3f view_position;
    math::vector3f world_position;
    math::vector3f normal;
    math::vector2f uv;
};

struct triangle_render {
public:
    vertex_data *vertex[3]; // this enables fast vertex swapping during compute
    vertex_data *left_edge_top, *right_edge_top;
    vertex_data *left_edge_delta, *right_edge_delta;
    int halftriangle_height;
    
    constexpr vertex_data &vtx(int i) { return *vertex[i]; }
    
    triangle_render();
    void prepare_edges(); // sort by y
    void prepare_upper_halftriangle(); // prepare l/r deltas and long edge midpoint
    void prepare_lower_halftriangle(); // prepare l/r deltas

private:
    void compute_delta(vertex_data &result, const vertex_data &start, const vertex_data &finish, float steps);
    
private:
    vertex_data edge_endpoint[3];
    vertex_data long_edge_midpoint;
    vertex_data short_edge_delta, long_edge_delta;
};

#endif
