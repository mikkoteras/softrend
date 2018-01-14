#ifndef MESH_H
#define MESH_H

#include "bounding_box.h"
#include "line.h"
#include "triangle.h"
#include "math/matrix.h"
#include "math/vector.h"
#include <vector>

class color3;
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
    unsigned add_vertex(const math::vector3f &v);
    unsigned add_vertex_normal(const math::vector3f &vn);
    unsigned add_line(unsigned v1, unsigned v2, const color3 &c1, const color3 &c2);
    unsigned add_line(const math::vector3f &v1, const math::vector3f &v2, const color3 &c1, const color3 &c2);
    unsigned add_triangle(unsigned vi1, unsigned vi2, unsigned vi3, unsigned ni1, unsigned ni2, unsigned ni3,
                          const math::vector2f &uv1, const math::vector2f &uv2, const math::vector2f &uv3,
                          const material *mat);
    unsigned add_triangle(unsigned vi1, unsigned vi2, unsigned vi3,
                          unsigned ni1, unsigned ni2, unsigned ni3, const material *mat);

public: // for scene, composition
    bool get_visibility() const;
    void set_visibility(bool val);
    void set_scaling(float s);
    void set_scaling(float x, float y, float z);
    void set_rotation(float x, float y, float z);
    void set_position(float x, float y, float z);

public: // for scene, rendering
    bounding_box local_bounding_box() const;
    const math::matrix4x4f &local_to_world();
    unsigned first_vertex_index() const;
    unsigned num_vertex_indices() const;
    unsigned first_normal_index() const;
    unsigned num_normal_indices() const;
    unsigned first_line_index() const;
    unsigned num_line_indices() const;
    unsigned first_triangle_index() const;
    unsigned num_triangle_indices() const;

private:
    scene *parent_scene;
    bool is_visible = true;

    // indices to scene's object store
    unsigned first_vertex = 0, num_vertices = 0;
    unsigned first_normal = 0, num_normals = 0;
    unsigned first_line = 0, num_lines = 0;
    unsigned first_triangle = 0, num_triangles = 0;
    
    math::matrix4x4f scaling = math::matrix4x4f::identity();
    math::matrix4x4f rotation = math::matrix4x4f::identity();
    math::matrix4x4f position = math::matrix4x4f::identity();
    math::matrix4x4f local_to_world_transformation = math::matrix4x4f::identity();
    bool local_to_world_transformation_dirty = true;
};

#endif
