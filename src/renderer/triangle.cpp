#include "triangle.h"
#include "color.h"
#include "framebuffer.h"
#include "math_util.h"
#include "mesh.h"
#include "scene.h"
#include <algorithm>

using namespace math;

triangle::triangle() {
}

triangle::triangle(int vi1, int vi2, int vi3,
                   const math::vector3f &uv1, const math::vector3f &uv2, const math::vector3f &uv3,
                   int ni1, int ni2, int ni3,
                   const texture *t) :
    tex(t) {

    vertex_index[0] = vi1;
    vertex_index[1] = vi2;
    vertex_index[2] = vi3;
    vertex_uv[0] = uv1;
    vertex_uv[1] = uv2;
    vertex_uv[2] = uv3;
    normal_index[0] = ni1;
    normal_index[1] = ni2;
    normal_index[2] = ni3;
}

triangle::triangle(const triangle &rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
        normal_index[i] = rhs.normal_index[i];
    }

    tex = rhs.tex;
}

triangle::triangle(triangle &&rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
        normal_index[i] = rhs.normal_index[i];
    }

    tex = rhs.tex;
}

const triangle &triangle::operator=(const triangle &rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
        normal_index[i] = rhs.normal_index[i];
    }

    tex = rhs.tex;
    return *this;
}

triangle &triangle::operator=(triangle &&rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
        normal_index[i] = rhs.normal_index[i];
    }

    tex = rhs.tex;
    return *this;
}

triangle::~triangle() {
}

void triangle::render(framebuffer &target, const mesh &parent, const scene &grandparent) const {
    const vector4f *view_data = parent.view_coordinate_data();

    // vertex winding test
    vector4f v1 = view_data[vertex_index[0]];
    vector4f v2 = view_data[vertex_index[1]];
    vector4f v3 = view_data[vertex_index[2]];

    float sum = (v2.x() - v1.x()) * (v2.y() + v1.y()) +
                (v3.x() - v2.x()) * (v3.y() + v2.y()) +
                (v1.x() - v3.x()) * (v1.y() + v3.y());

    if (sum < 0.0f)
        return;

    // plane clip
    if (grandparent.visible_volume().clip(v1, v2, v3))
        return;

    // shading
    const vector4f *normals = parent.world_normal_data();
    vector3f surface_normal = (normals[normal_index[0]] +
                               normals[normal_index[1]] +
                               normals[normal_index[2]]).dehomo().unit();

    color shade = grandparent.shade(surface_normal);
    edge e[3] = { create_edge(0, 1, view_data), create_edge(1, 2, view_data), create_edge(0, 2, view_data) };
    int long_edge_index = find_long_edge(e, view_data);

    draw_half_triangle(e[long_edge_index], e[(long_edge_index + 1) % 3], target, view_data, shade);
    draw_half_triangle(e[long_edge_index], e[(long_edge_index + 2) % 3], target, view_data, shade);
}

triangle::edge triangle::create_edge(int vi1, int vi2, const vector4f *vertex_data) const {
    if (vertex_data[vertex_index[vi1]].y() < vertex_data[vertex_index[vi2]].y())
        return edge { vi1, vi2 };
    else
        return edge { vi2, vi1 };
}

int triangle::find_long_edge(edge *edges, const vector4f *vertex_data) const {
    float height[3] = {
        (vertex_data[vertex_index[edges[0].bottom]].y() - vertex_data[vertex_index[edges[0].top]].y()),
        (vertex_data[vertex_index[edges[1].bottom]].y() - vertex_data[vertex_index[edges[1].top]].y()),
        (vertex_data[vertex_index[edges[2].bottom]].y() - vertex_data[vertex_index[edges[2].top]].y())
    };

    int i = height[0] > height[1] ? 0 : 1;
    return height[i] > height[2] ? i : 2;
}

void triangle::draw_half_triangle(const edge &long_edge, const edge &short_edge,
                                  framebuffer &target, const vector4f *vertex_data,
                                  const color &shade) const {
    // TODO: maybe use vector& rather than copy? Maybe create that render_context thingy?
    // long_edge is the one that needs two passes to draw, reaching from top y to bottom y.
    int long_top_y = vertex_data[vertex_index[long_edge.top]].y();
    int short_top_y = vertex_data[vertex_index[short_edge.top]].y();
    int long_bottom_y = vertex_data[vertex_index[long_edge.bottom]].y();
    int short_bottom_y = vertex_data[vertex_index[short_edge.bottom]].y();

    if (long_bottom_y == long_top_y || short_bottom_y == short_top_y)
        return;
    
    float height_1 = long_bottom_y - long_top_y;
    float height_2 = short_bottom_y - short_top_y;

    int long_top_x = vertex_data[vertex_index[long_edge.top]].x();
    float long_top_z = vertex_data[vertex_index[long_edge.top]].z();
    int long_bottom_x = vertex_data[vertex_index[long_edge.bottom]].x();
    float long_bottom_z = vertex_data[vertex_index[long_edge.bottom]].z();
    int short_top_x = vertex_data[vertex_index[short_edge.top]].x();
    float short_top_z = vertex_data[vertex_index[short_edge.top]].z();
    int short_bottom_x = vertex_data[vertex_index[short_edge.bottom]].x();
    float short_bottom_z = vertex_data[vertex_index[short_edge.bottom]].z();

    int y_offset = short_top_y - long_top_y; // 0 or rows already drawn
    float x1_delta = (long_bottom_x - long_top_x) / height_1;
    float x2_delta = (short_bottom_x - short_top_x) / height_2;
    float z1_delta = (long_bottom_z - long_top_z) / height_1;
    float z2_delta = (short_bottom_z - short_top_z) / height_2;
    float u1_delta = (vertex_uv[long_edge.bottom].x() - vertex_uv[long_edge.top].x()) / height_1;
    float u2_delta = (vertex_uv[short_edge.bottom].x() - vertex_uv[short_edge.top].x()) / height_2;
    float v1_delta = (vertex_uv[long_edge.bottom].y() - vertex_uv[long_edge.top].y()) / height_1;
    float v2_delta = (vertex_uv[short_edge.bottom].y() - vertex_uv[short_edge.top].y()) / height_2;
    float x1 = long_top_x + y_offset * x1_delta;
    float x2 = short_top_x;
    float z1 = long_top_z + y_offset * z1_delta;
    float z2 = short_top_z;
    float u1 = vertex_uv[long_edge.top].x() + y_offset * u1_delta;
    float u2 = vertex_uv[short_edge.top].x();
    float v1 = vertex_uv[long_edge.top].y() + y_offset * v1_delta;
    float v2 = vertex_uv[short_edge.top].y();
    float min_y = short_top_y;
    float max_y = short_bottom_y;
    float z, z_delta;
    float u, u_delta;
    float v, v_delta;

    if (min_y < 0) {
        x1 += -min_y * x1_delta;
        x2 += -min_y * x2_delta;
        z1 += -min_y * z1_delta;
        z2 += -min_y * z2_delta;
        u1 += -min_y * u1_delta;
        u2 += -min_y * u2_delta;
        v1 += -min_y * v1_delta;
        v2 += -min_y * v2_delta;
        min_y = 0;
    }

    max_y = std::min((int)max_y, target.pixel_height() - 1);

    // TODO: check z-coords as well.

    for (int y = min_y; y <= max_y; ++y) {
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

        if (min_x < 0) {
            z += -min_x * z_delta;
            u += -min_x * u_delta;
            v += -min_x * v_delta;
            min_x = 0;
        }

        max_x = std::min(max_x, target.pixel_width() - 1);

        for (int x = min_x; x <= max_x; ++x, z += z_delta, u += u_delta, v += v_delta)
            target.set_pixel_unchecked(x, y, z, shade * tex->at(u, v));

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
