#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "surface_position.h"
#include "types.h"
#include "math/matrix.h"
#include "math/vector.h"
#include <vector>

class framebuffer;
class light_list;
class material;
class mesh;
class scene;
struct scene_render_context;

class triangle {
public:
    triangle();
    triangle(unsigned vi1, unsigned vi2, unsigned vi3, unsigned ni1, unsigned ni2, unsigned ni3,
             const math::vector2f &uv1, const math::vector2f &uv2, const math::vector2f &uv3,
             const material *mat);
    triangle(unsigned vi1, unsigned vi2, unsigned vi3, unsigned ni1, unsigned ni2, unsigned ni3, const material *mat);
    triangle(const triangle &rhs);
    triangle(triangle &&rhs);
    const triangle &operator=(const triangle &rhs);
    triangle &operator=(triangle &&rhs);
    ~triangle();

public:
    const unsigned *vertex_indices() const;
    bool has_transparency() const;
    void prepare_for_render(const scene_render_context &scene_context);
    void render(const scene_render_context &scene_context, unsigned thread_index) const;

private:
    void prepare_halftriangles();
    bool triangle_winds_clockwise(const math::vector3f *view_coordinate_data) const;
    int compute_y_skip(int y, unsigned thread_index, unsigned num_threads) const;

public:
    void visualize_normals(const scene_render_context &scene_context);
    void visualize_reflection_vectors(const scene_render_context &scene_context);

private:
    template<combined_interpolation_mode_t mode>
    void render_triangle(const scene_render_context &scene_context, unsigned thread_index) const;

    template<combined_interpolation_mode_t mode>
    void render_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const;

private: // constant data
    unsigned vertex_index[3]; // indices to parent mesh's vertex data
    unsigned normal_index[3]; // indices to parent mesh's normal data
    math::vector2f vertex_uv[3];
    const material *mat;

    bool is_textured;
    bool has_distinct_normals;
    bool has_uv_coordinates;

private: // render data
    surface_position vertex[3];
    surface_position long_edge_midpoint;
    surface_position short_edge_delta[2], long_edge_delta;

    surface_position *left_edge_top[2], *right_edge_top[2];
    surface_position *left_edge_delta[2], *right_edge_delta[2];
    int halftriangle_height[2];

    bool skip_render;
};

#include "triangle.cpp.h"

#endif
