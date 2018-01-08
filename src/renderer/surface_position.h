#ifndef SURFACE_POSITION_H
#define SURFACE_POSITION_H

#include "color3.h"
#include "vector.h"

// All the data required for a triangle render at a point. Depending
// on the shading model and available data, many of the fields might
// not get used.
struct surface_position {
public:
    math::vector3f view_position; // TODO: x and y are mostly unnecessary
    math::vector3f world_position; // phong
    math::vector3f normal; // smooth phong polys
    math::vector2f uv; // textured polys
    color3 shade; // gouraud

public:
    // Add one step to the members enumerated in interpolation_mode
    template<unsigned interpolation_mode>
    void add(const surface_position &delta);

    // Add "multiplier" steps to the members enumerated in interpolation_mode
    template<unsigned interpolation_mode>
    void add(float multiplier, const surface_position &delta);

    // Compute a single step delta for the members enumerated in interpolation_mode
    template<unsigned interpolation_mode>
    void compute_delta(const surface_position &v1, const surface_position &v2, float steps);
    
public:
    std::string to_string() const;
};

#include "surface_position.cpp.h"

#endif
