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
    void render_flat(const scene_render_context &scene_context, unsigned thread_index) const;
    void render_gouraud(const scene_render_context &scene_context, unsigned thread_index) const;
    void render_phong(const scene_render_context &scene_context, unsigned thread_index) const;
    void render_smooth_phong(const scene_render_context &scene_context, unsigned thread_index) const;
    void render_flat_phong(const scene_render_context &scene_context, unsigned thread_index) const;

private:
    bool triangle_winds_clockwise() const;

    void render_colored_flat_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const;
    void render_colored_gouraud_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const;
    void render_colored_smooth_phong_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const;
    void render_colored_flat_phong_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const;
    
    void render_textured_flat_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const;
    void render_textured_gouraud_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const;
    void render_textured_smooth_phong_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const;
    void render_textured_flat_phong_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const;

public:
    void visualize_normals(const scene_render_context &scene_context);
    void visualize_reflection_vectors(const scene_render_context &scene_context);

private:
    shading_model_t compute_shading_limit();

private:
    surface_position &vtx(int i);
    const surface_position &vtx(int i) const;
    void prepare_edges(); // sort by y
    void prepare_halftriangles();
    int compute_y_skip(int y, size_t thread_index, size_t num_threads) const;
    
private: // constant data
    unsigned vertex_index[3]; // indices to parent mesh's vertex data
    unsigned normal_index[3]; // indices to parent mesh's normal data
    math::vector2f vertex_uv[3];
    const material *mat;

    bool has_distinct_normals;
    bool has_uv_coordinates;
    shading_model_t shading_limit;

private: // render data
    surface_position edge_endpoint[3];
    surface_position long_edge_midpoint;
    surface_position short_edge_delta[2], long_edge_delta;

    surface_position *vertex[3]; // this enables fast vertex swapping during compute
    surface_position *left_edge_top[2], *right_edge_top[2];
    surface_position *left_edge_delta[2], *right_edge_delta[2];
    int halftriangle_height[2];

    math::vector3f surface_normal; // only used when not interpolating normals
    math::vector3f surface_midpoint; // used with flat polys
    color3 shade; // used with flat gouraud polys
        
    bool skip_render;
};

#endif
