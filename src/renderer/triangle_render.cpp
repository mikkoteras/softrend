#include "triangle_render.h"
#include "light_list.h"
#include "texture.h"
#include "util.h"
#include <sstream>

void vertex_data::add_vwnts(const vertex_data &delta) {
    view_position += delta.view_position;
    world_position += delta.world_position;
    normal += delta.normal;
    uv += delta.uv;
    shade += delta.shade;
}

void vertex_data::add_vwnt(const vertex_data &delta) {
    view_position += delta.view_position;
    world_position += delta.world_position;
    normal += delta.normal;
    uv += delta.uv;
}

void vertex_data::add_vwt(const vertex_data &delta) {
    view_position += delta.view_position;
    world_position += delta.world_position;
    uv += delta.uv;
}

void vertex_data::add_vwn(const vertex_data &delta) {
    view_position += delta.view_position;
    world_position += delta.world_position;
    normal += delta.normal;
}

void vertex_data::add_vts(const vertex_data &delta) {
    view_position += delta.view_position;
    uv += delta.uv;
    shade += delta.shade;
}

void vertex_data::add_vw(const vertex_data &delta) {
    view_position += delta.view_position;
    world_position += delta.world_position;
}

void vertex_data::add_vt(const vertex_data &delta) {
    view_position += delta.view_position;
    uv += delta.uv;
}

void vertex_data::add_vs(const vertex_data &delta) {
    view_position += delta.view_position;
    shade += delta.shade;
}

void vertex_data::add_v(const vertex_data &delta) {
    view_position += delta.view_position;
}

void vertex_data::add_vwnts(float multiplier, const vertex_data &delta) {
    view_position += multiplier * delta.view_position;
    world_position += multiplier * delta.world_position;
    normal += multiplier * delta.normal;
    uv += multiplier * delta.uv;
    shade += multiplier * delta.shade;
}

void vertex_data::add_vwnt(float multiplier, const vertex_data &delta) {
    view_position += multiplier * delta.view_position;
    world_position += multiplier * delta.world_position;
    normal += multiplier * delta.normal;
    uv += multiplier * delta.uv;
}

void vertex_data::add_vts(float multiplier, const vertex_data &delta) {
    view_position += multiplier * delta.view_position;
    uv += multiplier * delta.uv;
    shade += multiplier * delta.shade;
}

void vertex_data::add_vwt(float multiplier, const vertex_data &delta) {
    view_position += multiplier * delta.view_position;
    world_position += multiplier * delta.world_position;
    uv += multiplier * delta.uv;
}

void vertex_data::add_vwn(float multiplier, const vertex_data &delta) {
    view_position += multiplier * delta.view_position;
    world_position += multiplier * delta.world_position;
    normal += multiplier * delta.normal;
}

void vertex_data::add_vw(float multiplier, const vertex_data &delta) {
    view_position += multiplier * delta.view_position;
    world_position += multiplier * delta.world_position;
}

void vertex_data::add_vt(float multiplier, const vertex_data &delta) {
    view_position += multiplier * delta.view_position;
    uv += multiplier * delta.uv;
}

void vertex_data::add_vs(float multiplier, const vertex_data &delta) {
    view_position += multiplier * delta.view_position;
    shade += multiplier * delta.shade;
}

void vertex_data::add_v(float multiplier, const vertex_data &delta) {
    view_position += multiplier * delta.view_position;
}

void vertex_data::compute_delta_vwnts(const vertex_data &v1, const vertex_data &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    world_position = (v2.world_position - v1.world_position) / steps;
    normal = (v2.normal - v1.normal) / steps;
    uv = (v2.uv - v1.uv) / steps;
    shade = (v2.shade - v1.shade) / steps;
}

void vertex_data::compute_delta_vwnt(const vertex_data &v1, const vertex_data &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    world_position = (v2.world_position - v1.world_position) / steps;
    normal = (v2.normal - v1.normal) / steps;
    uv = (v2.uv - v1.uv) / steps;
}

void vertex_data::compute_delta_vwt(const vertex_data &v1, const vertex_data &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    world_position = (v2.world_position - v1.world_position) / steps;
    uv = (v2.uv - v1.uv) / steps;
}

void vertex_data::compute_delta_vwn(const vertex_data &v1, const vertex_data &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    world_position = (v2.world_position - v1.world_position) / steps;
    normal = (v2.normal - v1.normal) / steps;
}

void vertex_data::compute_delta_vts(const vertex_data &v1, const vertex_data &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    uv = (v2.uv - v1.uv) / steps;
    shade = (v2.shade - v1.shade) / steps;
}

void vertex_data::compute_delta_vw(const vertex_data &v1, const vertex_data &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    world_position = (v2.world_position - v1.world_position) / steps;
}

void vertex_data::compute_delta_vt(const vertex_data &v1, const vertex_data &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    uv = (v2.uv - v1.uv) / steps;
}

void vertex_data::compute_delta_vs(const vertex_data &v1, const vertex_data &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    shade = (v2.shade - v1.shade) / steps;
}

void vertex_data::compute_delta_v(const vertex_data &v1, const vertex_data &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
}

std::string vertex_data::to_string() const {
    std::ostringstream o;
    o << "v = " << util::to_string(view_position)
      << " w = " << util::to_string(world_position)
      << " n = " << util::to_string(normal)
      << " uv = " << util::to_string(uv)
      << " s = " << shade.to_string();
    return o.str();
}

triangle_render::triangle_render() {
    for (int i = 0; i < 3; ++i)
        vertex[i] = &edge_endpoint[i];
}

void triangle_render::prepare_edges() {
    // round the view x/y positions to avoid gradient rounding artifacts
    // TODO could this be done during transformation already?

    for (int i = 0; i < 3; ++i) {
        edge_endpoint[i].view_position.x() = roundf(edge_endpoint[i].view_position.x());
        edge_endpoint[i].view_position.y() = roundf(edge_endpoint[i].view_position.y());
    }

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
    long_edge_delta.compute_delta_vwnts(*vertex[0], *vertex[2], bot_y - top_y); // TODO vwnts?
    
    if (halftriangle_height == 0) {
        long_edge_midpoint = *vertex[0];
        return; // zero scanlines to draw
    }

    float top_half_height = halftriangle_height;
    short_edge_delta.compute_delta_vwnts(*vertex[0], *vertex[1], top_half_height);

    left_edge_top = right_edge_top = vertex[0];
    long_edge_midpoint = *vertex[0];
    long_edge_midpoint.add_vwnts(top_half_height, long_edge_delta);
    
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
    short_edge_delta.compute_delta_vwnts(*vertex[1], *vertex[2], bot_half_height);

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
