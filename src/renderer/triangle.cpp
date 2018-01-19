#include "triangle.h"
#include "color3.h"
#include "framebuffer.h"
#include "light.h"
#include "light_list.h"
#include "material.h"
#include "scene.h"
#include "math/linear_transforms.h"
#include "math/math_util.h"
#include "math/matrix.h"
#include <algorithm>

using namespace math;

triangle::triangle() {
}

triangle::triangle(unsigned vi1, unsigned vi2, unsigned vi3, unsigned ni1, unsigned ni2, unsigned ni3,
                   const math::vector2f &uv1, const math::vector2f &uv2, const math::vector2f &uv3,
                   const material *mat) :
    vertex_index{vi1, vi2, vi3},
    normal_index{ni1, ni2, ni3},
    vertex_uv{uv1, uv2, uv3},
    mat(mat),
    is_textured(mat->is_textured()),
    has_distinct_normals(ni1 != ni2 || ni1 != ni3),
    has_uv_coordinates(true) {
}

triangle::triangle(unsigned vi1, unsigned vi2, unsigned vi3, unsigned ni1, unsigned ni2, unsigned ni3,
                   const material *mat) :
    vertex_index{vi1, vi2, vi3},
    normal_index{ni1, ni2, ni3},
    vertex_uv{vector2f(), vector2f(), vector2f()},
    mat(mat),
    is_textured(mat->is_textured()),
    has_distinct_normals(ni1 != ni2 || ni1 != ni3),
    has_uv_coordinates(false) {
}

triangle::triangle(const triangle &rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        normal_index[i] = rhs.normal_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
    }

    mat = rhs.mat;
    is_textured = rhs.is_textured;
    has_distinct_normals = rhs.has_distinct_normals;
    has_uv_coordinates = rhs.has_uv_coordinates;
}

triangle::triangle(triangle &&rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        normal_index[i] = rhs.normal_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
    }
        
    mat = rhs.mat;
    is_textured = rhs.is_textured;
    has_distinct_normals = rhs.has_distinct_normals;
    has_uv_coordinates = rhs.has_uv_coordinates;
}

const triangle &triangle::operator=(const triangle &rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        normal_index[i] = rhs.normal_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
    }

    mat = rhs.mat;
    is_textured = rhs.is_textured;
    has_distinct_normals = rhs.has_distinct_normals;
    has_uv_coordinates = rhs.has_uv_coordinates;

    return *this;
}

triangle &triangle::operator=(triangle &&rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        normal_index[i] = rhs.normal_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
    }

    mat = rhs.mat;
    is_textured = rhs.is_textured;
    has_distinct_normals = rhs.has_distinct_normals;
    has_uv_coordinates = rhs.has_uv_coordinates;

    return *this;
}

triangle::~triangle() {
}

const unsigned *triangle::vertex_indices() const {
    return vertex_index;
}

bool triangle::has_transparency() const {
    return mat->has_transparency();
}

void triangle::prepare_for_render(const scene_render_context &scene_context) {
    const vector3f *view_coord = scene_context.parent_scene->view_coordinate_data();

    skip_render = triangle_winds_clockwise(view_coord) ||
                  scene_context.parent_scene->visible_volume().clip(
                      view_coord[vertex_index[0]],
                      view_coord[vertex_index[1]],
                      view_coord[vertex_index[2]]);

    if (skip_render)
        return;
    
    int sorted_vertex_index[3] = { 0, 1, 2 }; // vertex indices in view y-order
    static const int lhs[3] = { 0, 1, 0 }; // compare pair
    static const int rhs[3] = { 1, 2, 1 }; // during sort

    // sort indices
    for (int i = 0; i < 3; ++i) {
        const vector3f &v1 = view_coord[vertex_index[sorted_vertex_index[lhs[i]]]];
        const vector3f &v2 = view_coord[vertex_index[sorted_vertex_index[rhs[i]]]];

        if (v1.y() > v2.y()) {
            int swap = sorted_vertex_index[lhs[i]];
            sorted_vertex_index[lhs[i]] = sorted_vertex_index[rhs[i]];
            sorted_vertex_index[rhs[i]] = swap;
        }
    }
    
    const vector3f *world_coord = scene_context.parent_scene->world_coordinate_data();
    const vector3f *world_normal = scene_context.parent_scene->world_normal_data();

    for (int i = 0; i < 3; ++i) {
        int si = sorted_vertex_index[i];
        int vi = vertex_index[si];
        int ni = normal_index[si];
        vertex[i].view_position = view_coord[vi];
        vertex[i].view_position.x() = roundf(vertex[i].view_position.x()); // TODO separate x, y, z and interpolate them
        vertex[i].view_position.y() = roundf(vertex[i].view_position.y());
        vertex[i].world_position = world_coord[vi];
        vertex[i].normal = world_normal[ni];
        vertex[i].uv = vertex_uv[si];
    }

    shading_model_t shading = scene_context.parent_scene->get_shading_model();
    
    if (shading == flat) {
        surface_position surface;
        surface.world_position = vector3f{0.0f, 0.0f, 0.0f};
        surface.normal = vector3f{0.0f, 0.0f, 0.0f};
    
        for (int i = 0; i < 3; ++i) {
            surface.world_position += vertex[i].world_position / (i + 1);
            surface.normal += vertex[i].normal;
        }

        surface.normal.normalize();
        surface.shade = mat->shade(surface, scene_context, false);

        for (int i = 0; i < 3; ++i)
            vertex[i].shade = surface.shade;
    }
    else if (shading == gouraud)
        for (int i = 0; i < 3; ++i)
            vertex[i].shade = mat->shade(vertex[i], scene_context, false);

    prepare_halftriangles();
}

void triangle::render(const scene_render_context &scene_context, unsigned thread_index) const {
    if (skip_render)
        return;

    shading_model_t shading = scene_context.parent_scene->get_shading_model();
    unsigned interpolation = interpolate_view_position;

    if (shading == phong) {
        interpolation |= interpolate_world_position;

        if (has_distinct_normals)
            interpolation |= interpolate_normal;
    }
    else if (shading == gouraud)
        interpolation |= interpolate_shade;

    if (is_textured)
        interpolation |= interpolate_uv;

    // This looks weird, but is actually [sic]. This is a mix
    // or compile time and runtime information.
    if (interpolation == colored_flat)
        render_triangle<colored_flat>(scene_context, thread_index);
    else if (interpolation == colored_gouraud)
        render_triangle<colored_gouraud>(scene_context, thread_index);
    else if (interpolation == colored_flat_phong)
        render_triangle<colored_flat_phong>(scene_context, thread_index);
    else if (interpolation == colored_smooth_phong)
        render_triangle<colored_smooth_phong>(scene_context, thread_index);
    else if (interpolation == textured_flat)
        render_triangle<textured_flat>(scene_context, thread_index);
    else if (interpolation == textured_gouraud)
        render_triangle<textured_gouraud>(scene_context, thread_index);
    else if (interpolation == textured_flat_phong)
        render_triangle<textured_flat_phong>(scene_context, thread_index);
    else if (interpolation == textured_smooth_phong)
        render_triangle<textured_smooth_phong>(scene_context, thread_index);
    else if (interpolation == full)
        render_triangle<full>(scene_context, thread_index);
}

void triangle::prepare_halftriangles() {
    int top_y = vertex[0].view_position.y();
    int mid_y = vertex[1].view_position.y();
    int bot_y = vertex[2].view_position.y();

    halftriangle_height[0] = mid_y - top_y;
    halftriangle_height[1] = bot_y - mid_y;
    long_edge_delta.compute_delta<full>(vertex[0], vertex[2], bot_y - top_y);

    if (halftriangle_height[0] > 0) {
        float top_half_height = halftriangle_height[0];
        short_edge_delta[0].compute_delta<full>(vertex[0], vertex[1], halftriangle_height[0]);

        left_edge_top[0] = right_edge_top[0] = &vertex[0];
        long_edge_midpoint = vertex[0];
        long_edge_midpoint.add<full>(top_half_height, long_edge_delta);
    
        if (long_edge_midpoint.view_position.x() <= vertex[1].view_position.x()) { // sort r/l
            left_edge_delta[0] = &long_edge_delta;
            right_edge_delta[0] = &short_edge_delta[0];
        }
        else {
            left_edge_delta[0] = &short_edge_delta[0];
            right_edge_delta[0] = &long_edge_delta;
        }

        halftriangle_height[0] -= 1; // Don't draw the middle line so it won't get drawn twice.
    }
    else
        long_edge_midpoint = vertex[0];

    if (halftriangle_height[1] > 0) {
        short_edge_delta[1].compute_delta<full>(vertex[1], vertex[2], halftriangle_height[1]);

        if (long_edge_midpoint.view_position.x() <= vertex[1].view_position.x()) { // sort r/l
            left_edge_top[1] = &long_edge_midpoint;
            left_edge_delta[1] = &long_edge_delta;
            right_edge_top[1] = &vertex[1];
            right_edge_delta[1] = &short_edge_delta[1];
        }
        else {
            left_edge_top[1] = &vertex[1];
            left_edge_delta[1] = &short_edge_delta[1];
            right_edge_top[1] = &long_edge_midpoint;
            right_edge_delta[1] = &long_edge_delta;
        }
    }
}

// Return the minimum number of scanlines to skip downward given starting
// scanline y and the thread index and count, such that the resulting scanline
// y + result is nonnegative and (y + result) % num_threads == thread_index.
int triangle::compute_y_skip(int y, unsigned thread_index, unsigned num_threads) const {
    int ti = static_cast<int>(thread_index);
    int nt = static_cast<int>(num_threads);

    if (y < 0)
        return ti - y;
    else
        return ((ti - (y % nt)) + nt) % nt;
}

bool triangle::triangle_winds_clockwise(const vector3f *view_coordinate_data) const {
    float x[3], y[3];
    
    for (int i = 0; i < 3; ++i) {
        x[i] = view_coordinate_data[vertex_index[i]].x();
        y[i] = view_coordinate_data[vertex_index[i]].y();
    }

    float sum = (x[1] - x[0]) * (y[1] + y[0]) +
                (x[2] - x[1]) * (y[2] + y[1]) +
                (x[0] - x[2]) * (y[0] + y[2]);

    return sum < 0.0f;
}

void triangle::visualize_normals(const scene_render_context &scene_context) {
    const vector3f *view_data = scene_context.parent_scene->view_coordinate_data();
    const vector3f *world_data = scene_context.parent_scene->world_coordinate_data();
    const vector3f *normal_data = scene_context.parent_scene->world_normal_data();
    const matrix4x4f &world_to_view = scene_context.parent_scene->world_to_view();
    color3 yellow(1.0f, 1.0f, 0.0f);
    vector3f mid_point, mid_normal;

    for (int i = 0; i < 3; ++i) {
        int vi = vertex_index[i], ni = normal_index[i];

        vector3f vertex(view_data[vi]); // view vertex
        vector3f world_normal(world_data[vi] + 0.3f * normal_data[ni]); // world normal, offset from vertex
        vector3f view_normal((world_to_view * world_normal.homo()).dehomo_with_divide());
        line::render(*scene_context.target, vertex.x(), vertex.y(), vertex.z(), yellow,
                     view_normal.x(), view_normal.y(), view_normal.z(), yellow);

        mid_point += (world_data[vi] - mid_point) / (i + 1);
        mid_normal += (normal_data[ni] - mid_normal) / (i + 1);
    }

    mid_normal.normalize();
    vector3f world_normal(mid_point + 0.3f * mid_normal); // world normal, offset from midpoint
    vector3f view_point((world_to_view * mid_point.homo()).dehomo_with_divide());
    vector3f view_normal((world_to_view * world_normal.homo()).dehomo_with_divide());

    line::render(*scene_context.target, view_point.x(), view_point.y(), view_point.z(), yellow,
                 view_normal.x(), view_normal.y(), view_normal.z(), yellow);
}

void triangle::visualize_reflection_vectors(const scene_render_context &scene_context) {
    const vector3f *world_data = scene_context.parent_scene->world_coordinate_data();
    const vector3f *normal_data = scene_context.parent_scene->world_normal_data();
    const matrix4x4f &world_to_view = scene_context.parent_scene->world_to_view();
    
    // triangle midpoint and the normal out of it
    vector3f mid_point, mid_normal;

    for (int i = 0; i < 3; ++i) {
        mid_point += (world_data[vertex_index[i]] - mid_point) / (i + 1);
        mid_normal += (normal_data[normal_index[i]] - mid_normal) / (i + 1);
    }

    mid_normal.normalize();
    vector3f midpoint_to_eye = (scene_context.parent_scene->get_eye_position() - mid_point).unit();

    // phong vectors for each light
    for (const light *source: scene_context.parent_scene->light_sources().get()) {
        vector3f light_vector(source->surface_to_light_unit(mid_point));
        float normal_dot_light(mid_normal.dot(light_vector));

        vector3f reflection_vector(2.0f * normal_dot_light * mid_normal - light_vector);
        vector3f p = (world_to_view * mid_point.homo()).dehomo_with_divide();
        vector3f l = (world_to_view * (mid_point + 0.3f * light_vector).homo()).dehomo_with_divide();
        vector3f r = (world_to_view * (mid_point + 0.3f * reflection_vector).homo()).dehomo_with_divide();

        color3 light_color(source->specular());
        color3 reflected_color(light_color * mat->get_specular_reflectivity());

        line::render(*scene_context.target, p.x(), p.y(), p.z(), light_color, l.x(), l.y(), l.z(), light_color);

        if (midpoint_to_eye.dot(reflection_vector) > 0.0f)
            line::render(*scene_context.target, p.x(), p.y(), p.z(), reflected_color, r.x(), r.y(), r.z(), reflected_color);
    }
}
