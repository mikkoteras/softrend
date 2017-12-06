#include "triangle_render.h"
#include "light_list.h"
#include "texture.h"

std::string vertex_data::to_string() const {
    std::ostringstream o;
    o << "v = " << util::to_string(view_position)
      << " w = " << util::to_string(world_position)
      << " n = " << util::to_string(normal)
      << " uv = " << util::to_string(uv);
    return o.str();
}

triangle_render::triangle_render() {
    for (int i = 0; i < 3; ++i)
        vertex[i] = &edge_endpoint[i];
}

void triangle_render::prepare_edges() {
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
    int top_y = vertex[0]->view_position.y();
    int mid_y = vertex[1]->view_position.y();
    int bot_y = vertex[2]->view_position.y();

    halftriangle_height = mid_y - top_y;
    compute_delta(long_edge_delta, *vertex[0], *vertex[2], bot_y - top_y);
    
    if (halftriangle_height == 0) {
        long_edge_midpoint = *vertex[0];
        return; // zero scanlines to draw
    }

    float top_half_height = halftriangle_height;
    compute_delta(short_edge_delta, *vertex[0], *vertex[1], top_half_height);

    long_edge_midpoint.view_position = vertex[0]->view_position + top_half_height * long_edge_delta.view_position;
    long_edge_midpoint.world_position = vertex[0]->world_position + top_half_height * long_edge_delta.world_position;
    long_edge_midpoint.normal = vertex[0]->normal + top_half_height * long_edge_delta.normal;
    long_edge_midpoint.uv = vertex[0]->uv + top_half_height * long_edge_delta.uv;

    left_edge_top = right_edge_top = vertex[0];
    
    if (long_edge_midpoint.view_position.x() <= vertex[1]->view_position.x()) { // sort r/l
        left_edge_delta = &long_edge_delta;
        right_edge_delta = &short_edge_delta;
    }
    else {
        left_edge_delta = &short_edge_delta;
        right_edge_delta = &long_edge_delta;
    }
}

void triangle_render::prepare_lower_halftriangle() {
    int mid_y = vertex[1]->view_position.y();
    int bot_y = vertex[2]->view_position.y();

    halftriangle_height = bot_y - mid_y;
    
    if (halftriangle_height == 0)
        return; // zero scanlines to draw

    float bot_half_height = halftriangle_height;

    compute_delta(short_edge_delta, *vertex[1], *vertex[2], bot_half_height);
    
    if (long_edge_midpoint.view_position.x() <= vertex[1]->view_position.x()) { // sort r/l
        left_edge_top = &long_edge_midpoint;
        left_edge_delta = &long_edge_delta;
        right_edge_top = vertex[1];
        right_edge_delta = &short_edge_delta;
    }
    else {
        left_edge_top = vertex[1];
        left_edge_delta = &short_edge_delta;
        right_edge_top = &long_edge_midpoint;
        right_edge_delta = &long_edge_delta;
    }
}

void triangle_render::compute_delta(vertex_data &result, const vertex_data &start, const vertex_data &finish, float steps) {
    result.view_position = (finish.view_position - start.view_position) / steps;
    result.world_position = (finish.world_position - start.world_position) / steps;
    result.normal = (finish.normal - start.normal) / steps;
    result.uv = (finish.uv - start.uv) / steps;
}
