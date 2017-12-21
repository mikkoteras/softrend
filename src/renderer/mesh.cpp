#include "mesh.h"
#include "color.h"
#include "linear_transforms.h"
#include "framebuffer.h"
#include "math_util.h"
#include "scene.h"
#include "texture.h"
#include "util.h"
#include "viewport_transforms.h"
#include <algorithm>

using namespace std;
using namespace math;

mesh::mesh(scene *parent_scene) :
    parent_scene(parent_scene),
    scaling(matrix4x4f::identity()),
    rotation(matrix4x4f::identity()),
    position(matrix4x4f::identity()),
    local_to_world_transformation_dirty(true)
{
}

mesh::mesh(const mesh &rhs) :
    parent_scene(rhs.parent_scene),
    local_coordinates(rhs.local_coordinates),
    local_normals(rhs.local_normals),
    local_to_world_transformation_dirty(true) {
}

mesh::mesh(mesh &&rhs) :
    parent_scene(rhs.parent_scene),
    local_coordinates(move(rhs.local_coordinates)),
    local_normals(move(rhs.local_normals)),
    local_to_world_transformation_dirty(true) {
}

const mesh &mesh::operator=(const mesh &rhs) {
    if (this != &rhs) {
        parent_scene(rhs.parent_scene);
        local_coordinates = rhs.local_coordinates;
        local_normals = rhs.local_normals;
        local_to_world_transformation_dirty = true;
    }

    return *this;
}

mesh &mesh::operator=(mesh &&rhs) {
    parent_scene(rhs.parent_scene);
    local_coordinates = move(rhs.local_coordinates);
    local_normals = move(rhs.local_normals);
    local_to_world_transformation_dirty = true;
    return *this;
}

mesh::~mesh() {
}

int mesh::add_vertex(const vector3f &v) {
    last_coordinate_index = parent_scene->add_vertex(v);

    if (first_coordinate_index < 0)
        first_coordinate_index = last_coordinate_index;
    
    return last_coordinate_index;
}

int mesh::add_vertex_normal(const vector3f &vn) {
    last_normal_index = parent_scene->add_normal(v);

    if (first_normal_index < 0)
        first_normal_index = last_normal_index;
    
    return last_normal_index;
}

void mesh::add_triangle(int vi1, int vi2, int vi3,
                        const vector3f &uv1, const vector3f &uv2, const vector3f &uv3,
                        int ni1, int ni2, int ni3,
                        const material *mat) {
    parent_scene->add_triangle(vi1 + first_coordinate_index, vi2 + first_coordinate_index, vi3 + first_coordinate_index,
                               ni1 + first_normal_index, ni2 + first_normal_index, ni3 + first_normal_index,
                               uv1, uv2, uv3,
                               mat);
}

void mesh::add_triangle(int vi1, int vi2, int vi3, int ni1, int ni2, int ni3, const material *mat) {
    parent_scene->add_triangle(vi1 + first_coordinate_index, vi2 + first_coordinate_index, vi3 + first_coordinate_index,
                               ni1 + first_normal_index, ni2 + first_normal_index, ni3 + first_normal_index,
                               uv1, uv2, uv3,
                               mat);
}

void mesh::add_line(int v1, int v2, const color &c1, const color &c2) {
    lines.push_back(line(v1, v2, c1, c2));
}

void mesh::add_line(const math::vector3f &v1, const math::vector3f &v2, const color &c1, const color &c2) {
    int vi1 = parent_scene->add_vertex(v1);
    int vi2 = parent_scene->add_vertex(v2);

    if (first_coordinate_index < 0)
        first_coordinate_index = vi1;

    last_coordinate_index = vi2;
    parent_scene->add_line(vi1, vi2, c1, c2);
}

void mesh::set_scaling(float x, float y, float z) {
    scaling = linear_transforms::scale3<float>(x, y, z);
    local_to_world_transformation_dirty = true;
}

void mesh::set_rotation(float x, float y, float z) {
    rotation =
        linear_transforms::rotate3z<float>(z) *
        linear_transforms::rotate3y<float>(y) *
        linear_transforms::rotate3x<float>(x);
    local_to_world_transformation_dirty = true;
}

void mesh::set_position(float x, float y, float z) {
    position = linear_transforms::translate3<float>(x, y, z);
    local_to_world_transformation_dirty = true;
}

bounding_box mesh::local_bounding_box() const {
    if (local_coordinates.empty())
        return bounding_box(vector3f{0, 0, 0});
    else {
        bounding_box result(local_coordinates[0]);

        for (unsigned i = 1; i < local_coordinates.size(); ++i)
            result.stretch(local_coordinates[i]);

        return result;
    }
}

const matrix4x4f &mesh::local_to_world() const {
    if (local_to_world_transformation_dirty) {
        local_to_world_transformation = position * rotation * scaling;
        local_to_world_transformation_dirty = false;
    }

    return local_to_world_transformation;
}

int mesh::min_coordinate_index() const {
    return first_coordinate_index;
}

int mesh::max_coordinate_index() const {
    return last_coordinate_index;
}

int mesh::min_normal_index() const {
    return first_normal_index;
}

int mesh::max_normal_index() const {
    return last_normal_index;
}
