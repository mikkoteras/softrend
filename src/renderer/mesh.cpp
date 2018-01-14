#include "mesh.h"
#include "color3.h"
#include "framebuffer.h"
#include "scene.h"
#include "texture.h"
#include "util.h"
#include "math/linear_transforms.h"
#include "math/math_util.h"
#include "math/viewport_transforms.h"

using namespace std;
using namespace math;

mesh::mesh(scene *parent_scene) :
    parent_scene(parent_scene) {

    parent_scene->add_mesh(this);
}

mesh::~mesh() {
}

unsigned mesh::add_vertex(const vector3f &v) {
    unsigned index = parent_scene->add_vertex(v);
    
    if (++num_vertices == 1)
        first_vertex = index;

    return index;
}

unsigned mesh::add_vertex_normal(const vector3f &vn) {
    unsigned index = parent_scene->add_vertex_normal(vn);

    if (++num_normals == 1)
        first_normal = index;

    return index;
}

unsigned mesh::add_line(unsigned v1, unsigned v2, const color3 &c1, const color3 &c2) {
    unsigned index = parent_scene->add_line(v1, v2, c1, c2);

    if (++num_lines == 1)
        first_line = index;

    return index;
}

unsigned mesh::add_line(const math::vector3f &v1, const math::vector3f &v2, const color3 &c1, const color3 &c2) {
    unsigned vi1 = add_vertex(v1);
    unsigned vi2 = add_vertex(v2);
    return add_line(vi1, vi2, c1, c2);
}

unsigned mesh::add_triangle(unsigned vi1, unsigned vi2, unsigned vi3,
                            unsigned ni1, unsigned ni2, unsigned ni3,
                            const vector2f &uv1, const vector2f &uv2, const vector2f &uv3,
                            const material *mat) {
    unsigned index = parent_scene->add_triangle(vi1 + first_vertex,
                                                vi2 + first_vertex,
                                                vi3 + first_vertex,
                                                ni1 + first_normal,
                                                ni2 + first_normal,
                                                ni3 + first_normal,
                                                uv1,
                                                uv2,
                                                uv3,
                                                mat);

    if (++num_triangles == 1)
        first_triangle = index;

    return index;
}

unsigned mesh::add_triangle(unsigned vi1, unsigned vi2, unsigned vi3,
                            unsigned ni1, unsigned ni2, unsigned ni3, const material *mat) {
    unsigned index = parent_scene->add_triangle(vi1 + first_vertex,
                                                vi2 + first_vertex,
                                                vi3 + first_vertex,
                                                ni1 + first_normal,
                                                ni2 + first_normal,
                                                ni3 + first_normal,
                                                mat);

    if (++num_triangles == 1)
        first_triangle = index;

    return index;
}

bool mesh::get_visibility() const {
    return is_visible;
}

void mesh::set_visibility(bool val) {
    is_visible = val;
}

void mesh::set_scaling(float s) {
    scaling = linear_transforms::scale3<float>(s);
    local_to_world_transformation_dirty = true;
}

void mesh::set_scaling(float x, float y, float z) {
    scaling = linear_transforms::scale3<float>(x, y, z);
    local_to_world_transformation_dirty = true;
}

void mesh::set_rotation(float x, float y, float z) {
    rotation = linear_transforms::rotate3z<float>(z) *
               linear_transforms::rotate3y<float>(y) *
               linear_transforms::rotate3x<float>(x);
    local_to_world_transformation_dirty = true;
}

void mesh::set_position(float x, float y, float z) {
    position = linear_transforms::translate3<float>(x, y, z);
    local_to_world_transformation_dirty = true;
}

bounding_box mesh::local_bounding_box() const {
    if (num_vertices > 0) {
        const vector4f *vertices = parent_scene->local_coordinate_data();
        bounding_box result(vertices[first_vertex]);

        for (unsigned i = 1; i < num_vertices; ++i)
            result.stretch(vertices[first_vertex + i]);

        return result;
    }
    else
        return bounding_box(vector3f{0, 0, 0});
}

const matrix4x4f &mesh::local_to_world() {
    if (local_to_world_transformation_dirty) {
        local_to_world_transformation = position * rotation * scaling;
        local_to_world_transformation_dirty = false;
    }

    return local_to_world_transformation;
}

unsigned mesh::first_vertex_index() const {
    return first_vertex;
}

unsigned mesh::num_vertex_indices() const {
    return is_visible ? num_vertices : 0;
}

unsigned mesh::first_normal_index() const {
    return first_normal;
}

unsigned mesh::num_normal_indices() const {
    return is_visible ? num_normals : 0;
}

unsigned mesh::first_line_index() const {
    return first_line;
}

unsigned mesh::num_line_indices() const {
    return is_visible ? num_lines : 0;
}

unsigned mesh::first_triangle_index() const {
    return first_triangle;
}

unsigned mesh::num_triangle_indices() const {
    return is_visible ? num_triangles : 0;
}
