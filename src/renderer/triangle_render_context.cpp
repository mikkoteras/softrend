#include "triangle_render_context.h"
#include "light_list.h"
#include "util.h"
#include <cmath>

triangle_render_context::triangle_render_context() {
    for (int i = 0; i < 3; ++i)
        vertex[i] = &edge_endpoint[i];
}

triangle_render_context::~triangle_render_context() {
}

surface_position &triangle_render_context::vtx(int i) {
    return *vertex[i];
}

const surface_position &triangle_render_context::vtx(int i) const {
    return *vertex[i];
}

void triangle_render_context::prepare_edges() {
    // round the view x/y positions to avoid gradient rounding artifacts
    // TODO could this be done during transformation already?

    for (int i = 0; i < 3; ++i) {
        edge_endpoint[i].view_position.x() = roundf(edge_endpoint[i].view_position.x());
        edge_endpoint[i].view_position.y() = roundf(edge_endpoint[i].view_position.y());
    }

    if (vtx(0).view_position.y() > vtx(1).view_position.y()) {
        surface_position *tmp = vertex[0];
        vertex[0] = vertex[1];
        vertex[1] = tmp;
    }

    if (vtx(1).view_position.y() > vtx(2).view_position.y()) {
        surface_position *tmp = vertex[1];
        vertex[1] = vertex[2];
        vertex[2] = tmp;

        if (vtx(0).view_position.y() > vtx(1).view_position.y()) {
            surface_position *tmp = vertex[0];
            vertex[0] = vertex[1];
            vertex[1] = tmp;
        }
    }
}

void triangle_render_context::prepare_upper_halftriangle() {
    int top_y = vertex[0]->view_position.y();
    int mid_y = vertex[1]->view_position.y();
    int bot_y = vertex[2]->view_position.y();

    halftriangle_height[0] = mid_y - top_y;
    long_edge_delta.compute_delta<full>(*vertex[0], *vertex[2], bot_y - top_y);
    
    if (halftriangle_height == 0) {
        long_edge_midpoint = *vertex[0];
        return; // zero scanlines to draw
    }

    float top_half_height = halftriangle_height[0];
    short_edge_delta[0].compute_delta<full>(*vertex[0], *vertex[1], top_half_height);

    left_edge_top[0] = right_edge_top[0] = vertex[0];
    long_edge_midpoint = *vertex[0];
    long_edge_midpoint.add<full>(top_half_height, long_edge_delta);
    
    if (long_edge_midpoint.view_position.x() <= vertex[1]->view_position.x()) { // sort r/l
        left_edge_delta[0] = &long_edge_delta;
        right_edge_delta[0] = &short_edge_delta[0];
    }
    else {
        left_edge_delta[0] = &short_edge_delta[0];
        right_edge_delta[0] = &long_edge_delta;
    }

    halftriangle_height[0] -= 1; // Don't draw the middle line so it won't get drawn twice.
}

void triangle_render_context::prepare_lower_halftriangle() {
    int mid_y = vertex[1]->view_position.y();
    int bot_y = vertex[2]->view_position.y();
    halftriangle_height[1] = bot_y - mid_y;

    if (halftriangle_height == 0)
        return; // no scanlines to draw

    float bot_half_height = halftriangle_height[1];
    short_edge_delta[1].compute_delta<full>(*vertex[1], *vertex[2], bot_half_height);

    if (long_edge_midpoint.view_position.x() <= vertex[1]->view_position.x()) { // sort r/l
        left_edge_top[1] = &long_edge_midpoint;
        left_edge_delta[1] = &long_edge_delta;
        right_edge_top[1] = vertex[1];
        right_edge_delta[1] = &short_edge_delta[1];
    }
    else {
        left_edge_top[1] = vertex[1];
        left_edge_delta[1] = &short_edge_delta[1];
        right_edge_top[1] = &long_edge_midpoint;
        right_edge_delta[1] = &long_edge_delta;
    }
}

int triangle_render_context::compute_y_skip(int y, size_t thread_index, size_t num_threads) const {
    if (y < 0)
        return thread_index - y;
    else
        return ((thread_index - (y % num_threads)) + num_threads) % num_threads;
}
