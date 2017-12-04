#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "matrix.h"
#include "triangle_render.h"
#include "vector.h"

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
             const math::vector3f &uv1, const math::vector3f &uv2, const math::vector3f &uv3,
             int ni1, int ni2, int ni3,
             const material *mat);
    triangle(int vi1, int vi2, int vi3, int ni1, int ni2, int ni3, const material *mat);
    triangle(const triangle &rhs);
    triangle(triangle &&rhs);
    const triangle &operator=(const triangle &rhs);
    triangle &operator=(triangle &&rhs);
    ~triangle();

public:
    void render(framebuffer &target, const mesh &parent_mesh, const scene &parent_scene) const;
    void render2(framebuffer &target, const mesh &parent_mesh, const scene &parent_scene) const;
    void visualize_normals(framebuffer &target, const mesh &parent_mesh,
                           scene &parent_scene, const math::matrix4x4f &local_to_view) const;
    void visualize_reflection_vectors(framebuffer &target, const mesh &parent_mesh,
                                      scene &parent_scene, const math::matrix4x4f &local_to_view) const;



private:
    static triangle_render render_context;
    static bool triangle_winds_clockwise();

    void fill_phong();
    
private: // render helpers
    struct edge { int top, bottom; }; // indices to vertex_index
    edge create_edge(int vi1, int vi2, const math::vector4f *vertex_data) const;
    int find_long_edge(edge *edges, const math::vector4f *vertex_data) const;
    void draw_half_triangle(const edge &long_edge, const edge &short_edge, framebuffer &target,
                            const mesh &parent_mesh, const scene &parent_scene) const;

private:
    int vertex_index[3]; // indices to parent mesh's vertex data
    math::vector3f vertex_uv[3];
    int normal_index[3]; // indices to parent mesh's normal data
    const material *mat;
};

#endif
