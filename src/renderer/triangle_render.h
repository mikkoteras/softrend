#ifndef TRIANGLE_RENDER_H
#define TRIANGLE_RENDER_H

#include "vector.h"
#include <string>

class light_list;
class texture;

// All the data required for a triangle render. Depending on the shading
// model and available data, many of the fields might not get used.
struct vertex_data {
    math::vector3f view_position;
    math::vector3f world_position;
    math::vector3f normal; // only used when interpolating normals
    math::vector2f uv;

public:
    // legend: v = view coords, w = world coords, n = surface normals, t = texture coords
    void add_vwnt(const vertex_data &delta);
    void add_vwt(const vertex_data &delta);
    void add_vwn(const vertex_data &delta);
    void add_vw(const vertex_data &delta);
    void add_vt(const vertex_data &delta);
    void add_v(const vertex_data &delta);

    void add_vwnt(float multiplier, const vertex_data &delta); // lhs += multiplier * delta
    void add_vwt(float multiplier, const vertex_data &delta);  // used for clipping
    void add_vwn(float multiplier, const vertex_data &delta);
    void add_vw(float multiplier, const vertex_data &delta);
    void add_vt(float multiplier, const vertex_data &delta);
    void add_v(float multiplier, const vertex_data &delta);

    void compute_delta_vwnt(const vertex_data &v1, const vertex_data &v2, float steps);
    void compute_delta_vwt(const vertex_data &v1, const vertex_data &v2, float steps);
    void compute_delta_vwn(const vertex_data &v1, const vertex_data &v2, float steps);
    void compute_delta_vw(const vertex_data &v1, const vertex_data &v2, float steps);
    void compute_delta_vt(const vertex_data &v1, const vertex_data &v2, float steps);
    void compute_delta_v(const vertex_data &v1, const vertex_data &v2, float steps);

    std::string to_string() const;
};

struct triangle_render {
public:
    vertex_data *vertex[3]; // this enables fast vertex swapping during compute
    vertex_data *left_edge_top, *right_edge_top;
    vertex_data *left_edge_delta, *right_edge_delta;
    int halftriangle_height;

    math::vector3f eye;
    math::vector3f surface_normal; // only used when not interpolating normals
    math::vector3f surface_midpoint; // used with flat polys
    const light_list *lights;
    const texture *tex;

    constexpr vertex_data &vtx(int i) { return *vertex[i]; }
    
    triangle_render();
    void prepare_edges(); // sort by y

    void prepare_upper_halftriangle();
    void prepare_lower_halftriangle();
    
private:
    vertex_data edge_endpoint[3];
    vertex_data long_edge_midpoint;
    vertex_data short_edge_delta, long_edge_delta;
};

#endif
