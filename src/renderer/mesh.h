#ifndef MESH_H
#define MESH_H

#include "color.h"
#include "matrix.h"
#include "triangle.h"
#include "vector.h"
#include "vertex.h"
#include <vector>

class framebuffer;
class scene;
class texture;

class mesh {
public:
    mesh();
    mesh(const mesh &rhs);
    mesh(mesh &&rhs);
    const mesh &operator=(const mesh &rhs);
    mesh &operator=(mesh &&rhs);
    ~mesh();
    
    int add_vertex(const math::vector4f &v, const color &c);
    int add_vertex_normal(const math::vector4f &vn);
    int add_texture_coordinates(const math::vector3f &tc);
    void add_triangle(int v1, int v2, int v3, int n1, int n2, int n3, int t1, int t2, int t3, const texture *tex);
    void add_triangle(int vi1, int vi2, int vi3,
                      const math::vector3f &uv1, const math::vector3f &uv2, const math::vector3f &uv3,
                      const texture *tex);
    void set_scaling(float x, float y, float z);
    void set_rotation(float x, float y, float z);
    void set_position(float x, float y, float z);

    void render(scene &sc, framebuffer &fb);
    void draw_texture_breakup(framebuffer &fb, int tex_width, int tex_height) const;

public: // for child geometries (like triangle)
    const math::vector4f *projected_vertices() const;
    
private:
    struct face {
        face(int vx1, int vx2, int vx3, int vn1, int vn2, int vn3, int tx1, int tx2, int tx3, const texture *t) :
            v1(vx1), v2(vx2), v3(vx3),
            n1(vn1), n2(vn2), n3(vn3),
            t1(tx1), t2(tx2), t3(tx3),
            tex(t) {
        }
        int v1, v2, v3; // vertex indices
        int n1, n2, n3; // vertex normal indices
        int t1, t2, t3; // texture uvw indices
        const texture *tex;
    };
    
private:
    std::vector<math::vector4f> points;
    std::vector<math::vector4f> normals;
    std::vector<math::vector3f> texture_coords;
    std::vector<color> colors;
    math::matrix4x4f scaling;
    math::matrix4x4f rotation;
    math::matrix4x4f position;
    std::vector<face> face_list;

    std::vector<math::vector4f> projected_points;
    std::vector<triangle> triangles;
};

#endif
