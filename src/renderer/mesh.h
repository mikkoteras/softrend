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
    void add_triangle(int vi1, int vi2, int vi3,  // indices to vertex coordinate data
                      const math::vector3f &uv1, const math::vector3f &uv2, const math::vector3f &uv3,  // texture uv coords
                      int ni1, int ni2, int ni3,  // indices to vertex normal data
                      const texture *tex);
    void set_scaling(float x, float y, float z);
    void set_rotation(float x, float y, float z);
    void set_position(float x, float y, float z);

    void render(scene &sc, framebuffer &fb);

public: // for child geometries (like triangle)
    const math::vector4f *world_coordinate_data() const;
    const math::vector4f *world_normal_data() const;
    const math::vector4f *view_coordinate_data() const;
    
private:
    std::vector<math::vector4f> local_coordinates;
    std::vector<math::vector4f> local_normals;
    std::vector<triangle> triangles;
    std::vector<color> colors;
    math::matrix4x4f scaling;
    math::matrix4x4f rotation;
    math::matrix4x4f position;
    std::vector<math::vector4f> world_coordinates;
    std::vector<math::vector4f> world_normals;
    std::vector<math::vector4f> view_coordinates;
};

#endif
