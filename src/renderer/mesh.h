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
    mesh() = delete;
    mesh(scene *parent_scene);
    mesh(const mesh &rhs);
    mesh(mesh &&rhs);
    const mesh &operator=(const mesh &rhs);
    mesh &operator=(mesh &&rhs);
    ~mesh();

public: // for geometry creation (mostly importer)
    int add_vertex(const math::vector3f &v);
    int add_vertex_normal(const math::vector3f &vn);
    void add_triangle(int vi1, int vi2, int vi3,
                      int ni1, int ni2, int ni3,
                      const math::vector3f &uv1, const math::vector3f &uv2, const math::vector3f &uv3,
                      const material *mat);
    void add_triangle(int vi1, int vi2, int vi3, int ni1, int ni2, int ni3, const material *mat);
    void add_line(int v1, int v2, const color &c1, const color &c2);
    void add_line(const math::vector3f &v1, const math::vector3f &v2, const color &c1, const color &c2);

public: // for scene, composition
    void set_scaling(float x, float y, float z);
    void set_rotation(float x, float y, float z);
    void set_position(float x, float y, float z);

public: // for scene, rendering
    bounding_box local_bounding_box() const;
    const math::matrix4x4f &local_to_world() const;
    int min_coordinate_index() const;
    int max_coordinate_index() const;
    int min_normal_index() const;
    int max_normal_index() const;

private:
    scene *parent_scene;

    int first_coordinate_index = -1; // coordinate and normal
    int last_coordinate_index = -1;  // indices in parent scene's
    int first_normal_index = -1;     // data store that belong to
    int last_normal_index = -1;      // this mesh
    
    math::matrix4x4f scaling;
    math::matrix4x4f rotation;
    math::matrix4x4f position;
    math::matrix4x4f local_to_world_transformation;
    bool local_to_world_transformation_dirty;
};

#endif
