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

    triangle_render();
    constexpr vertex_data &vtx(int i) { return *vertex[i]; }
    void sort_by_y();
    void prepare_upper_halftriangle();
    void prepare_lower_halftriangle();
    
private:
    vertex_data data[3];
};

#endif
