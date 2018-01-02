#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "matrix.h"
#include "triangle_render.h"
#include "types.h"
#include "vector.h"
#include <vector>

class color;
class framebuffer;
class light_list;
class material;
class mesh;
class scene;

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
    void render(framebuffer &target, const scene &parent_scene) const;

private:
    void render_flat(framebuffer &target, const scene &parent_scene) const;
    void render_gouraud(framebuffer &target, const scene &parent_scene) const;
    void render_phong(framebuffer &target, const scene &parent_scene) const;
    void render_smooth_phong(framebuffer &target, const scene &parent_scene) const;
    void render_flat_phong(framebuffer &target, const scene &parent_scene) const;

private:
    static triangle_render render_context;
    static bool triangle_winds_clockwise();

    void render_colored_flat_halftriangle(framebuffer &target) const;
    void render_colored_gouraud_halftriangle(framebuffer &target) const;
    void render_colored_smooth_phong_halftriangle(framebuffer &target) const;
    void render_colored_flat_phong_halftriangle(framebuffer &target) const;
    
    void render_textured_flat_halftriangle(framebuffer &target) const;
    void render_textured_gouraud_halftriangle(framebuffer &target) const;
    void render_textured_smooth_phong_halftriangle(framebuffer &target) const;
    void render_textured_flat_phong_halftriangle(framebuffer &target) const;

public:
    void visualize_normals(framebuffer &target, scene &parent_scene) const;
    void visualize_reflection_vectors(framebuffer &target, scene &parent_scene) const;
    
private:
    shading_model_t compute_shading_limit();

private:
    int vertex_index[3]; // indices to parent mesh's vertex data
    math::vector2f vertex_uv[3];
    int normal_index[3]; // indices to parent mesh's normal data
    const material *mat;

    bool has_distinct_normals;
    bool has_uv_coordinates;
    shading_model_t shading_limit;
};

#endif
