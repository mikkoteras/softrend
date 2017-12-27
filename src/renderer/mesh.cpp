#include "mesh.h"
#include "color.h"
#include "linear_transforms.h"
#include "framebuffer.h"
#include "math_util.h"
#include "scene.h"
#include "texture.h"
#include "util.h"
#include "viewport_transforms.h"

using namespace std;
using namespace math;

mesh::mesh(scene *parent_scene) :
    parent_scene(parent_scene) {

    parent_scene->add_mesh(this);
}

mesh::~mesh() {
}

int mesh::add_vertex(const vector3f &v) {
    bool first_vertex = last_vertex_index < 0;
    last_vertex_index = parent_scene->add_vertex(v);

    if (first_vertex)
        first_vertex_index = last_vertex_index;

    return last_vertex_index;
}

int mesh::add_vertex_normal(const vector3f &vn) {
    bool first_normal = last_normal_index < 0;
    last_normal_index = parent_scene->add_vertex_normal(vn);

    if (first_normal)
        first_normal_index = last_normal_index;
    
    return last_normal_index;
}

int mesh::add_line(int v1, int v2, const color &c1, const color &c2) {
    bool first_line = last_line_index < 0;
    last_line_index = parent_scene->add_line(v1, v2, c1, c2);

    if (first_line)
        first_line_index = last_line_index;

    return last_line_index;
}

int mesh::add_line(const math::vector3f &v1, const math::vector3f &v2, const color &c1, const color &c2) {
    int vi1 = add_vertex(v1);
    int vi2 = add_vertex(v2);
    return add_line(vi1, vi2, c1, c2);
}

int mesh::add_triangle(int vi1, int vi2, int vi3,
                        int ni1, int ni2, int ni3,
                        const vector3f &uv1, const vector3f &uv2, const vector3f &uv3,
                        const material *mat) {
    bool first_triangle = last_triangle_index < 0;
    last_triangle_index = parent_scene->add_triangle(vi1 + first_vertex_index,
                                                     vi2 + first_vertex_index,
                                                     vi3 + first_vertex_index,
                                                     ni1 + first_normal_index,
                                                     ni2 + first_normal_index,
                                                     ni3 + first_normal_index,
                                                     uv1,
                                                     uv2,
                                                     uv3,
                                                     mat);

    if (first_triangle)
        first_triangle_index = last_triangle_index;

    return last_triangle_index;
}

int mesh::add_triangle(int vi1, int vi2, int vi3, int ni1, int ni2, int ni3, const material *mat) {
    bool first_triangle = last_triangle_index < 0;
    last_triangle_index =  parent_scene->add_triangle(vi1 + first_vertex_index,
                                                      vi2 + first_vertex_index,
                                                      vi3 + first_vertex_index,
                                                      ni1 + first_normal_index,
                                                      ni2 + first_normal_index,
                                                      ni3 + first_normal_index,
                                                      mat);

    if (first_triangle)
        first_triangle_index = last_triangle_index;

    return last_triangle_index;
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
    if (first_vertex_index >= 0) {
        const vector4f *vertices = parent_scene->local_coordinate_data();
        bounding_box result(vertices[first_vertex_index]);

        for (int i = first_vertex_index + 1; i <= last_vertex_index; ++i)
            result.stretch(vertices[i]);

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

int mesh::min_vertex_index() const {
    return is_visible ? first_vertex_index : 0;
}

int mesh::max_vertex_index() const {
    return is_visible ? last_vertex_index : -1;
}

int mesh::min_normal_index() const {
    return is_visible ? first_normal_index : 0;
}

int mesh::max_normal_index() const {
    return is_visible ? last_normal_index : -1;
}

int mesh::min_line_index() const {
    return is_visible ? first_line_index : 0;
}

int mesh::max_line_index() const {
    return is_visible ? last_line_index : -1;
}

int mesh::min_triangle_index() const {
    return is_visible ? first_triangle_index : 0;
}

int mesh::max_triangle_index() const {
    return is_visible ? last_triangle_index : -1;
}
