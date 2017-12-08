#include "mesh.h"
#include "color.h"
#include "linear_transforms.h"
#include "framebuffer.h"
#include "math_util.h"
#include "scene.h"
#include "texture.h"
#include "util.h"
#include <algorithm>

#include <iostream>

using namespace std;
using namespace math;

mesh::mesh() :
    scaling(matrix4x4f::identity()),
    rotation(matrix4x4f::identity()),
    position(matrix4x4f::identity()) {
}

mesh::mesh(const mesh &rhs) :
    local_coordinates(rhs.local_coordinates),
    local_normals(rhs.local_normals),
    triangles(rhs.triangles),
    lines(rhs.lines),
    scaling(rhs.scaling),
    rotation(rhs.rotation),
    position(rhs.position),
    world_coordinates(rhs.world_coordinates),
    world_normals(rhs.world_normals),
    view_coordinates(rhs.view_coordinates) {
}

mesh::mesh(mesh &&rhs) :
    local_coordinates(move(rhs.local_coordinates)),
    local_normals(move(rhs.local_normals)),
    triangles(move(rhs.triangles)),
    lines(move(rhs.lines)),
    scaling(move(rhs.scaling)),
    rotation(move(rhs.rotation)),
    position(move(rhs.position)),
    world_coordinates(move(rhs.world_coordinates)),
    world_normals(move(rhs.world_normals)),
    view_coordinates(move(rhs.view_coordinates)) {
}

const mesh &mesh::operator=(const mesh &rhs) {
    if (this != &rhs) {
        local_coordinates = rhs.local_coordinates;
        local_normals = rhs.local_normals;
        triangles = rhs.triangles;
        lines = rhs.lines;
        scaling = rhs.scaling;
        rotation = rhs.rotation;
        position = rhs.position;
        world_coordinates = rhs.world_coordinates;
        world_normals = rhs.world_normals;
        view_coordinates = rhs.view_coordinates;
    }

    return *this;
}

mesh &mesh::operator=(mesh &&rhs) {
    local_coordinates = move(rhs.local_coordinates);
    local_normals = move(rhs.local_normals);
    triangles = move(rhs.triangles);
    lines = move(rhs.lines);
    scaling = move(rhs.scaling);
    rotation = move(rhs.rotation);
    position = move(rhs.position);
    world_coordinates = move(rhs.world_coordinates);
    world_normals = move(rhs.world_normals);
    view_coordinates = move(rhs.view_coordinates);
    return *this;
}

mesh::~mesh() {
}

int mesh::add_vertex(const vector3f &v) {
    local_coordinates.push_back(vector4f(v, 1.0f));
    world_coordinates.push_back(vector4f());
    view_coordinates.push_back(vector4f());
    return local_coordinates.size() - 1;
}

int mesh::add_vertex_normal(const vector3f &vn) {
    local_normals.push_back(vector4f(vn, 0.0f));
    world_normals.push_back(vector4f());
    return local_normals.size() - 1;
}

void mesh::add_triangle(int vi1, int vi2, int vi3,
                        const vector3f &uv1, const vector3f &uv2, const vector3f &uv3,
                        int ni1, int ni2, int ni3,
                        const material *mat) {
    triangles.push_back(triangle(vi1, vi2, vi3, uv1, uv2, uv3, ni1, ni2, ni3, mat));
}

void mesh::add_triangle(int vi1, int vi2, int vi3, int ni1, int ni2, int ni3, const material *mat) {
    triangles.push_back(triangle(vi1, vi2, vi3, ni1, ni2, ni3, mat));
}

void mesh::add_line(int v1, int v2, const color &c1, const color &c2) {
    lines.push_back(line(v1, v2, c1, c2));
}

void mesh::add_line(const math::vector3f &v1, const math::vector3f &v2, const color &c1, const color &c2) {
    int vi1 = add_vertex(v1);
    int vi2 = add_vertex(v2);
    lines.push_back(line(vi1, vi2, c1, c2));
}

void mesh::set_scaling(float x, float y, float z) {
    scaling = linear_transforms::scale3<float>(x, y, z);
}

void mesh::set_rotation(float x, float y, float z) {
    rotation =
        linear_transforms::rotate3z<float>(z) *
        linear_transforms::rotate3y<float>(y) *
        linear_transforms::rotate3x<float>(x);
}

void mesh::set_position(float x, float y, float z) {
    position = linear_transforms::translate3<float>(x, y, z);
}

void mesh::render(scene &sc, framebuffer &fb, bool visualize_normals, bool visualize_reflection_vectors) {
    using namespace math::linear_transforms;
    float min_axis = fb.pixel_width() < fb.pixel_height() ? fb.pixel_width() : fb.pixel_height();

    // TODO: these hardly belong here
    matrix4x4f scale_to_screen_coords = scale3<float>(min_axis / 2.0f);
    matrix4x4f translate_to_screen_coords = translate3<float>(fb.pixel_width() / 2.0f, fb.pixel_height() / 2.0f, 0.0f);

    matrix4x4f local_to_world = position * rotation * scaling;
    matrix4x4f world_to_view = translate_to_screen_coords * scale_to_screen_coords * sc.world_to_view();

    for (unsigned i = 0, max = local_coordinates.size(); i < max; ++i) {
        world_coordinates[i] = local_to_world * local_coordinates[i];
        view_coordinates[i] = world_to_view * world_coordinates[i];
        view_coordinates[i].divide_by_h();
    }

    for (unsigned i = 0, max = local_normals.size(); i < max; ++i) {
        world_normals[i] = local_to_world * local_normals[i];
        world_normals[i].normalize();
    }

    for (const triangle &t: triangles)
        t.render(fb, *this, sc);

    for (const line &l: lines)
        l.render(fb, *this, sc);

    if (visualize_normals)
        for (const triangle &t: triangles)
            t.visualize_normals(fb, *this, sc, world_to_view);

    if (visualize_reflection_vectors) {
        unsigned step = triangles.size() / 25;
        step = max(step, 1u);

        for (unsigned i = 0; i < triangles.size(); i += step)
            triangles[i].visualize_reflection_vectors(fb, *this, sc, world_to_view);
    }
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

const vector4f *mesh::world_coordinate_data() const {
    return world_coordinates.data();}

const vector4f *mesh::world_normal_data() const {
    return world_normals.data();
}

const vector4f *mesh::view_coordinate_data() const {
    return view_coordinates.data();
}
