#ifndef SURFACE_POSITION_H
#define SURFACE_POSITION_H

#include "color3.h"
#include "vector.h"

// TODO: refactor using templates!

// All the data required for a triangle render at a point. Depending
// on the shading model and available data, many of the fields might
// not get used.
struct surface_position {
public:
    math::vector3f view_position; // TODO: x and y are mostly unnecessary
    math::vector3f world_position;
    math::vector3f normal; // only used when interpolating normals
    math::vector2f uv;
    color3 shade; // only used with gouraud

public:
    // legend: v = view coords, w = world coords, n = surface normals, t = texture coords, s = shade
    void add_vwnts(const surface_position &delta);
    void add_vwnt(const surface_position &delta);
    void add_vwt(const surface_position &delta);
    void add_vwn(const surface_position &delta);
    void add_vw(const surface_position &delta);
    void add_vts(const surface_position &delta);
    void add_vt(const surface_position &delta);
    void add_vs(const surface_position &delta);
    void add_v(const surface_position &delta);

    void add_vwnts(float multiplier, const surface_position &delta); // lhs += multiplier * delta
    void add_vwnt(float multiplier, const surface_position &delta);  // used for clipping
    void add_vwt(float multiplier, const surface_position &delta);
    void add_vwn(float multiplier, const surface_position &delta);
    void add_vts(float multiplier, const surface_position &delta);
    void add_vw(float multiplier, const surface_position &delta);
    void add_vt(float multiplier, const surface_position &delta);
    void add_vs(float multiplier, const surface_position &delta);
    void add_v(float multiplier, const surface_position &delta);

    void compute_delta_vwnts(const surface_position &v1, const surface_position &v2, float steps);
    void compute_delta_vwnt(const surface_position &v1, const surface_position &v2, float steps);
    void compute_delta_vwt(const surface_position &v1, const surface_position &v2, float steps);
    void compute_delta_vwn(const surface_position &v1, const surface_position &v2, float steps);
    void compute_delta_vw(const surface_position &v1, const surface_position &v2, float steps);
    void compute_delta_vts(const surface_position &v1, const surface_position &v2, float steps);
    void compute_delta_vt(const surface_position &v1, const surface_position &v2, float steps);
    void compute_delta_vs(const surface_position &v1, const surface_position &v2, float steps);
    void compute_delta_v(const surface_position &v1, const surface_position &v2, float steps);

    std::string to_string() const;
};

#endif
