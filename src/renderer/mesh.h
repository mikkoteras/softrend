#ifndef MESH_H
#define MESH_H

#include "bounding_box.h"
#include "line.h"
#include "matrix.h"
#include "triangle.h"
#include "vector.h"
#include <vector>

class color;
class material;
class scene;

class mesh {
public:
    mesh(scene *parent_scene);
    ~mesh();

    mesh() = delete;
    mesh(const mesh&) = delete;
    mesh(mesh&&) = delete;
    const mesh &operator=(const mesh&) = delete;
    mesh &operator=(mesh&&) = delete;

public: // for geometry creation (mostly importer)
    int add_vertex(const math::vector3f &v);
    int add_vertex_normal(const math::vector3f &vn);
    int add_line(int v1, int v2, const color &c1, const color &c2);
    int add_line(const math::vector3f &v1, const math::vector3f &v2, const color &c1, const color &c2);
    int add_triangle(int vi1, int vi2, int vi3,
                     int ni1, int ni2, int ni3,
                     const math::vector3f &uv1, const math::vector3f &uv2, const math::vector3f &uv3,
                     const material *mat);
    int add_triangle(int vi1, int vi2, int vi3, int ni1, int ni2, int ni3, const material *mat);

public: // for scene, composition
    bool get_visibility() const;
    void set_visibility(bool val);
    void set_scaling(float x, float y, float z);
    void set_rotation(float x, float y, float z);
    void set_position(float x, float y, float z);

public: // for scene, rendering
    bounding_box local_bounding_box() const;
    const math::matrix4x4f &local_to_world();
    int min_vertex_index() const;
    int max_vertex_index() const;
    int min_normal_index() const;
    int max_normal_index() const;
    int min_line_index() const;
    int max_line_index() const;
    int min_triangle_index() const;
    int max_triangle_index() const;

private:
    scene *parent_scene;
    bool is_visible = true;

    // indices to scene's object store
    int first_vertex_index = 0, last_vertex_index = -1;
    int first_normal_index = 0, last_normal_index = -1;
    int first_line_index = 0, last_line_index = -1;
    int first_triangle_index = 0, last_triangle_index = -1;
    
    math::matrix4x4f scaling = math::matrix4x4f::identity();
    math::matrix4x4f rotation = math::matrix4x4f::identity();
    math::matrix4x4f position = math::matrix4x4f::identity();
    math::matrix4x4f local_to_world_transformation = math::matrix4x4f::identity();
    bool local_to_world_transformation_dirty = true;
};

#endif
