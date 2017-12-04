#include "triangle_render.h"

triangle_render::triangle_render() {
    for (int i = 0; i < 3; ++i)
        vertex[i] = &data[i];
}

void triangle_render::sort_by_y() {
    if (vtx(0).view_position.y() > vtx(1).view_position.y()) {
        vertex_data *tmp = vertex[0];
        vertex[0] = vertex[1];
        vertex[1] = tmp;
    }

    if (vtx(1).view_position.y() > vtx(2).view_position.y()) {
        vertex_data *tmp = vertex[1];
        vertex[1] = vertex[2];
        vertex[2] = tmp;

        if (vtx(0).view_position.y() > vtx(1).view_position.y()) {
            vertex_data *tmp = vertex[0];
            vertex[0] = vertex[1];
            vertex[1] = tmp;
        }
    }
}

void triangle_render::prepare_upper_halftriangle() {
    
}

void triangle_render::prepare_lower_halftriangle() {
}
