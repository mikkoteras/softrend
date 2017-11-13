#include "mesh.h"
#include "linear_transforms.h"
#include "framebuffer.h"
#include "math_util.h"
#include "scene.h"
#include "texture.h"
#include "util.h"

using namespace std;
using namespace math;

mesh::mesh() :
    points(),
    normals(),
    texture_coords(),
    colors(),
    scaling(matrix4x4f::identity()),
    rotation(matrix4x4f::identity()),
    position(matrix4x4f::identity()),
    face_list(),
    projected_points(),
    triangles() {
}

mesh::mesh(const mesh &rhs) :
    points(rhs.points),
    normals(rhs.normals),
    texture_coords(rhs.texture_coords),
    colors(rhs.colors),
    scaling(rhs.scaling),
    rotation(rhs.rotation),
    position(rhs.position),
    face_list(rhs.face_list),
    projected_points(rhs.projected_points),
    triangles(rhs.triangles) {    
}

mesh::mesh(mesh &&rhs) : 
    points(move(rhs.points)),
    normals(move(rhs.normals)),
    texture_coords(move(rhs.texture_coords)),
    colors(move(rhs.colors)),
    scaling(move(rhs.scaling)),
    rotation(move(rhs.rotation)),
    position(move(rhs.position)),
    face_list(move(rhs.face_list)),
    projected_points(move(rhs.projected_points)),
    triangles(move(rhs.triangles)) {
}

const mesh &mesh::operator=(const mesh &rhs) {
    if (this != &rhs) {
        points = rhs.points;
        normals = rhs.normals;
        texture_coords = rhs.texture_coords;
        colors = rhs.colors;
        scaling = rhs.scaling;
        rotation = rhs.rotation;
        position = rhs.position;
        face_list = rhs.face_list;
        projected_points = rhs.projected_points;
        triangles = rhs.triangles;
    }

    return *this;
}

mesh &mesh::operator=(mesh &&rhs) {
    points = move(rhs.points);
    normals = move(rhs.normals);
    texture_coords = move(rhs.texture_coords);
    colors = move(rhs.colors);
    scaling = move(rhs.scaling);
    rotation = move(rhs.rotation);
    position = move(rhs.position);
    face_list = move(rhs.face_list);
    projected_points = move(rhs.projected_points);
    triangles = move(rhs.triangles);
    
    return *this;
}

mesh::~mesh() {
}

int mesh::add_vertex(const vector4f &v, const color &c) {
    points.push_back(v);
    colors.push_back(c);
    projected_points.push_back(vector4f());
    return points.size() - 1;
}

int mesh::add_vertex_normal(const vector4f &vn) {
    normals.push_back(vn);
    return normals.size() - 1;
}

int mesh::add_texture_coordinates(const vector3f &tc) {
    texture_coords.push_back(tc);
    return texture_coords.size() - 1;
}

void mesh::add_triangle(int v1, int v2, int v3, int n1, int n2, int n3, int t1, int t2, int t3, const texture *tex) {
    face_list.push_back(face(v1, v2, v3, n1, n2, n3, t1, t2, t3, tex));
}

void mesh::add_triangle(int vi1, int vi2, int vi3,
                        const vector3f &uv1, const vector3f &uv2, const vector3f &uv3,
                        const texture *tex) {
    triangles.push_back(triangle(vi1, vi2, vi3, uv1, uv2, uv3, tex));
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

#include <iostream>

void mesh::render(scene &sc, framebuffer &fb) {
    using namespace math::linear_transforms;
    float min_axis = fb.pixel_width() < fb.pixel_height() ? fb.pixel_width() : fb.pixel_height();

    // TODO: these hardly belong here
    matrix4x4f translate_to_screen_coords = translate3<float>(fb.pixel_width() / 2.0f, fb.pixel_height() / 2.0f, 0.0f);
    matrix4x4f scale_to_screen_coords = scale3<float>(min_axis / 2.0f);
    
    matrix4x4f local_to_view = translate_to_screen_coords * scale_to_screen_coords * sc.world_to_view() * position * rotation * scaling;
    std::vector<vector4f> view_points;
    
    for (unsigned i = 0, max = points.size(); i < max; ++i) {
        projected_points[i] = local_to_view * points[i];
        projected_points[i].divide_by_h();
    }

    for (const triangle &t: triangles)
        t.render(fb, *this);
}

void mesh::draw_texture_breakup(framebuffer &fb, int tex_width, int tex_height) const {
    color c(.8f, .2f, .2f, 1);

    for (unsigned i = 0; i < face_list.size(); ++i) {
        vector3f p1 = texture_coords[face_list[i].t1];
        vector3f p2 = texture_coords[face_list[i].t2];
        vector3f p3 = texture_coords[face_list[i].t3];

        int p1x = p1.x() * tex_width;
        int p2x = p2.x() * tex_width;
        int p3x = p3.x() * tex_width;

        int p1y = p1.y() * tex_height;
        int p2y = p2.y() * tex_height;
        int p3y = p3.y() * tex_height;

        fb.draw_line(p1x, p1y, c, p2x, p2y, c);
        fb.draw_line(p2x, p2y, c, p3x, p3y, c);
        fb.draw_line(p3x, p3y, c, p1x, p1y, c);
    }
}

const vector4f *mesh::projected_vertices() const {
    return projected_points.data();
}
