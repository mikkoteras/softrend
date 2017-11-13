#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "texture.h"
#include "vector.h"

class framebuffer;
class mesh;

class triangle {
public:
    triangle();
    triangle(int vi1, int vi2, int vi3,
             const math::vector3f &uv1, const math::vector3f &uv2, const math::vector3f &uv3,
             const texture *t);
    triangle(const triangle &rhs);
    triangle(triangle &&rhs);
    const triangle &operator=(const triangle &rhs);
    triangle &operator=(triangle &&rhs);
    ~triangle();

public:
    void render(framebuffer &target, const mesh &parent) const;
    
private: // render helpers
    struct edge { int top, bottom; }; // indices to vertex_index
    edge create_edge(int vi1, int vi2, const math::vector4f *vertex_data) const;
    int find_long_edge(edge *edges, const math::vector4f *vertex_data) const;
    void draw_half_triangle(const edge &long_edge, const edge &short_edge,
                            framebuffer &target, const math::vector4f *vertex_data) const;
    
private:
    int vertex_index[3]; // indices to mesh vertex data
    math::vector3f vertex_uv[3];
    const texture *tex;
};

#endif
