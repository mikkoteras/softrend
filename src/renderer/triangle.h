#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "triangle_render_context.h"
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
    triangle(int vi1, int vi2, int vi3,
             int ni1, int ni2, int ni3,
             const math::vector2f &uv1, const math::vector2f &uv2, const math::vector2f &uv3,
             const material *mat);
    triangle(int vi1, int vi2, int vi3, int ni1, int ni2, int ni3, const material *mat);
    triangle(const triangle &rhs);
    triangle(triangle &&rhs);
    const triangle &operator=(const triangle &rhs);
    triangle &operator=(triangle &&rhs);
    ~triangle();

public:
    const int *vertex_indices() const;
    bool has_transparency() const;
    void prepare_for_render(const scene_render_context &scene_context);
    void render(const scene_render_context &scene_context, int thread_index) const;

private:
    void render_flat(const scene_render_context &scene_context, int thread_index) const;
    void render_gouraud(const scene_render_context &scene_context, int thread_index) const;
    void render_phong(const scene_render_context &scene_context, int thread_index) const;
    void render_smooth_phong(const scene_render_context &scene_context, int thread_index) const;
    void render_flat_phong(const scene_render_context &scene_context, int thread_index) const;

private:
    bool triangle_winds_clockwise() const;

    void render_colored_flat_halftriangle(const scene_render_context &scene_context, int thread_index, int triangle_half) const;
    void render_colored_gouraud_halftriangle(const scene_render_context &scene_context, int thread_index, int triangle_half) const;
    void render_colored_smooth_phong_halftriangle(const scene_render_context &scene_context, int thread_index, int triangle_half) const;
    void render_colored_flat_phong_halftriangle(const scene_render_context &scene_context, int thread_index, int triangle_half) const;
    
    void render_textured_flat_halftriangle(const scene_render_context &scene_context, int thread_index, int triangle_half) const;
    void render_textured_gouraud_halftriangle(const scene_render_context &scene_context, int thread_index, int triangle_half) const;
    void render_textured_smooth_phong_halftriangle(const scene_render_context &scene_context, int thread_index, int triangle_half) const;
    void render_textured_flat_phong_halftriangle(const scene_render_context &scene_context, int thread_index, int triangle_half) const;

public:
    void visualize_normals(const scene_render_context &scene_context);
    void visualize_reflection_vectors(const scene_render_context &scene_context);

private:
    shading_model_t compute_shading_limit();

private:
    int vertex_index[3]; // indices to parent mesh's vertex data
    int normal_index[3]; // indices to parent mesh's normal data
    math::vector2f vertex_uv[3];
    const material *mat;

    bool has_distinct_normals;
    bool has_uv_coordinates;
    shading_model_t shading_limit;

    triangle_render_context render_context;
    bool skip_render;
};

#endif
