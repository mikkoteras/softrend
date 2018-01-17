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

#include <iostream>

triangle::triangle() {
}

triangle::triangle(unsigned vi1, unsigned vi2, unsigned vi3, unsigned ni1, unsigned ni2, unsigned ni3,
                   const math::vector2f &uv1, const math::vector2f &uv2, const math::vector2f &uv3,
                   const material *mat) :
    vertex_index{vi1, vi2, vi3},
    normal_index{ni1, ni2, ni3},
    vertex_uv{uv1, uv2, uv3},
    mat(mat),
    has_distinct_normals(ni1 != ni2 || ni1 != ni3),
    has_uv_coordinates(true),
    shading_limit(compute_shading_limit()) {
}

triangle::triangle(unsigned vi1, unsigned vi2, unsigned vi3, unsigned ni1, unsigned ni2, unsigned ni3,
                   const material *mat) :
    vertex_index{vi1, vi2, vi3},
    normal_index{ni1, ni2, ni3},
    vertex_uv{vector2f(), vector2f(), vector2f()},
    mat(mat),
    has_distinct_normals(ni1 != ni2 || ni1 != ni3),
    has_uv_coordinates(false),
    shading_limit(compute_shading_limit()) {
}

triangle::triangle(const triangle &rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        normal_index[i] = rhs.normal_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
    }

    mat = rhs.mat;
    has_distinct_normals = rhs.has_distinct_normals;
    has_uv_coordinates = rhs.has_uv_coordinates;
    shading_limit = rhs.shading_limit;
}

triangle::triangle(triangle &&rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        normal_index[i] = rhs.normal_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
    }
        
    mat = rhs.mat;
    has_distinct_normals = rhs.has_distinct_normals;
    has_uv_coordinates = rhs.has_uv_coordinates;
    shading_limit = rhs.shading_limit;
}

const triangle &triangle::operator=(const triangle &rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        normal_index[i] = rhs.normal_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
    }

    mat = rhs.mat;
    has_distinct_normals = rhs.has_distinct_normals;
    has_uv_coordinates = rhs.has_uv_coordinates;
    shading_limit = rhs.shading_limit;

    return *this;
}

triangle &triangle::operator=(triangle &&rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        normal_index[i] = rhs.normal_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
    }

    mat = rhs.mat;
    has_distinct_normals = rhs.has_distinct_normals;
    has_uv_coordinates = rhs.has_uv_coordinates;
    shading_limit = rhs.shading_limit;

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
    surface_midpoint = vector3f{0.0f, 0.0f, 0.0f};
    surface_normal = vector3f{0.0f, 0.0f, 0.0f};

    for (int i = 0; i < 3; ++i) {
        int si = sorted_vertex_index[i];
        int vi = vertex_index[si];
        int ni = normal_index[si];
        vertex[i].view_position = view_coord[vi];
        vertex[i].world_position = world_coord[vi];
        vertex[i].normal = world_normal[ni];
        vertex[i].uv = vertex_uv[i];

        surface_midpoint += vertex[i].world_position / (i + 1);
        surface_normal += vertex[i].normal;
    }

    if (shading_limit >= gouraud && scene_context.parent_scene->get_shading_model() == gouraud)
        for (int i = 0; i < 3; ++i)
            vertex[i].shade = mat->shade(vertex[i].world_position,
                                         vertex[i].normal,
                                         (scene_context.eye - vertex[i].world_position).unit(),
                                         *scene_context.light_sources);

    surface_normal /= 3.0f;
    prepare_halftriangles();
}

void triangle::render(const scene_render_context &scene_context, unsigned thread_index) const {
    if (skip_render)
        return;

    shading_model_t shading = scene_context.parent_scene->get_shading_model();

    if (shading == flat || shading_limit == flat)
        render_flat(scene_context, thread_index);
    else if (shading == gouraud || shading_limit == gouraud)
        render_gouraud(scene_context, thread_index);
    else
        render_phong(scene_context, thread_index);
}

void triangle::render_flat(const scene_render_context &scene_context, unsigned thread_index) const {
    if (mat->is_textured()) {
        render_textured_flat_halftriangle(scene_context, thread_index, 0);
        render_textured_flat_halftriangle(scene_context, thread_index, 1);
    }
    else {
        render_colored_flat_halftriangle(scene_context, thread_index, 0);
        render_colored_flat_halftriangle(scene_context, thread_index, 1);
    }
}

void triangle::render_gouraud(const scene_render_context &scene_context, unsigned thread_index) const {
    if (mat->is_textured()) {
        render_textured_gouraud_halftriangle(scene_context, thread_index, 0);
        render_textured_gouraud_halftriangle(scene_context, thread_index, 1);
    }
    else {
        render_colored_gouraud_halftriangle(scene_context, thread_index, 0);
        render_colored_gouraud_halftriangle(scene_context, thread_index, 1);
    }
}

void triangle::render_phong(const scene_render_context &scene_context, unsigned thread_index) const {
    if (has_distinct_normals)
        render_smooth_phong(scene_context, thread_index);
    else
        render_flat_phong(scene_context, thread_index);
}

void triangle::render_smooth_phong(const scene_render_context &scene_context, unsigned thread_index) const {
    if (mat->is_textured() && has_uv_coordinates) {
        render_textured_smooth_phong_halftriangle(scene_context, thread_index, 0);
        render_textured_smooth_phong_halftriangle(scene_context, thread_index, 1);
    }
    else {
        render_colored_smooth_phong_halftriangle(scene_context, thread_index, 0);
        render_colored_smooth_phong_halftriangle(scene_context, thread_index, 1);
    }
}

void triangle::render_flat_phong(const scene_render_context &scene_context, unsigned thread_index) const {
    if (mat->is_textured() && has_uv_coordinates) {
        render_textured_flat_phong_halftriangle(scene_context, thread_index, 0);
        render_textured_flat_phong_halftriangle(scene_context, thread_index, 1);
    }
    else {
        render_colored_flat_phong_halftriangle(scene_context, thread_index, 0);
        render_colored_flat_phong_halftriangle(scene_context, thread_index, 1);
    }
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

void triangle::render_colored_flat_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = colored_flat;

    if (halftriangle_height[triangle_half] <= 0)
        return;

    surface_position left = *left_edge_top[triangle_half];
    surface_position right = *right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);

    int y_skip = compute_y_skip(y, thread_index, scene_context.num_threads);

    if (y_skip != 0) {
        left.add<mode>(y_skip, *left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *right_edge_delta[triangle_half]);
        y += y_skip;
    }

    color4 shade(mat->shade(surface_midpoint,
                            surface_normal,
                            (scene_context.eye - surface_midpoint).unit(),
                            *scene_context.light_sources));

    for (int step = static_cast<int>(scene_context.num_threads); y <= max_y; y += step) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta<mode>(left, right, max_x - x);
        max_x = std::min(max_x, scene_context.target->pixel_width() - 1);

        if (x < 0) {
            pixel.add<mode>(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            scene_context.target->set_pixel_with_z_clip(x, y, pixel.view_position.z(), shade);
            pixel.add<mode>(delta); // TODO: skip view_position, use z alone
        }

        left.add<mode>(scene_context.num_threads, *left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *right_edge_delta[triangle_half]);
    }
}

void triangle::render_colored_gouraud_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = colored_gouraud;
    
    if (halftriangle_height[triangle_half] <= 0)
        return;

    surface_position left = *left_edge_top[triangle_half];
    surface_position right = *right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);

    int y_skip = compute_y_skip(y, thread_index, scene_context.num_threads);

    if (y_skip != 0) {
        left.add<mode>(y_skip, *left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *right_edge_delta[triangle_half]);
        y += y_skip;
    }
    
    for (int step = static_cast<int>(scene_context.num_threads); y <= max_y; y += step) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta<mode>(left, right, max_x - x);
        max_x = std::min(max_x, scene_context.target->pixel_width() - 1);

        if (x < 0) {
            pixel.add<mode>(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            scene_context.target->set_pixel_with_z_clip(x, y, pixel.view_position.z(), pixel.shade);
            pixel.add<mode>(delta); // TODO: skip view_position, use z alone
        }

        left.add<mode>(scene_context.num_threads, *left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *right_edge_delta[triangle_half]);
    }
}

void triangle::render_colored_smooth_phong_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = colored_smooth_phong;
    
    if (halftriangle_height[triangle_half] <= 0)
        return;

    surface_position left = *left_edge_top[triangle_half];
    surface_position right = *right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);

    int y_skip = compute_y_skip(y, thread_index, scene_context.num_threads);

    if (y_skip != 0) {
        left.add<mode>(y_skip, *left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *right_edge_delta[triangle_half]);
        y += y_skip;
    }

    for (int step = static_cast<int>(scene_context.num_threads); y <= max_y; y += step) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta<mode>(left, right, max_x - x);
        max_x = std::min(max_x, scene_context.target->pixel_width() - 1);

        if (x < 0) {
            pixel.add<mode>(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            if (scene_context.target->depth_at(x, y) < pixel.view_position.z())
                scene_context.target->set_pixel_overwriting_z_buffer(x, y, pixel.view_position.z(),
                                                                     mat->shade(pixel.world_position,
                                                                                pixel.normal.unit(),
                                                                                (scene_context.eye - pixel.world_position).unit(),
                                                                                *scene_context.light_sources));
            
            pixel.add<mode>(delta); // TODO: skip view_position, use z alone
        }

        left.add<mode>(scene_context.num_threads, *left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *right_edge_delta[triangle_half]);
    }
}

void triangle::render_colored_flat_phong_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = colored_flat_phong;
    
    if (halftriangle_height[triangle_half] <= 0)
        return;

    surface_position left = *left_edge_top[triangle_half];
    surface_position right = *right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);

    int y_skip = compute_y_skip(y, thread_index, scene_context.num_threads);

    if (y_skip != 0) {
        left.add<mode>(y_skip, *left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *right_edge_delta[triangle_half]);
        y += y_skip;
    }

    for (int step = static_cast<int>(scene_context.num_threads); y <= max_y; y += step) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta<mode>(left, right, max_x - x);
        max_x = std::min(max_x, scene_context.target->pixel_width() - 1);

        if (x < 0) {
            pixel.add<mode>(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            if (scene_context.target->depth_at(x, y) < pixel.view_position.z())
                scene_context.target->set_pixel_overwriting_z_buffer(x, y, pixel.view_position.z(),
                                                                     mat->shade(pixel.world_position,
                                                                                surface_normal,
                                                                                (scene_context.eye - pixel.world_position).unit(),
                                                                                pixel.uv,
                                                                                *scene_context.light_sources));

            pixel.add<mode>(delta); // TODO: skip view_position, use z alone
        }

        left.add<mode>(scene_context.num_threads, *left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *right_edge_delta[triangle_half]);
    }
}

void triangle::render_textured_flat_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = textured_flat;
    
    if (halftriangle_height[triangle_half] <= 0)
        return;

    surface_position left = *left_edge_top[triangle_half];
    surface_position right = *right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);
    
    int y_skip = compute_y_skip(y, thread_index, scene_context.num_threads);

    if (y_skip != 0) {
        left.add<mode>(y_skip, *left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *right_edge_delta[triangle_half]);
        y += y_skip;
    }
    color4 shade(mat->shade(surface_midpoint,
                            surface_normal,
                            (scene_context.eye - surface_midpoint).unit(),
                            *scene_context.light_sources));

    for (int step = static_cast<int>(scene_context.num_threads); y <= max_y; y += step) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta<mode>(left, right, max_x - x);
        max_x = std::min(max_x, scene_context.target->pixel_width() - 1);

        if (x < 0) {
            pixel.add<mode>(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            if (scene_context.target->depth_at(x, y) < pixel.view_position.z())
                scene_context.target->set_pixel_overwriting_z_buffer(x, y, pixel.view_position.z(),
                                                                     shade * mat->diffuse_texture_map(pixel.uv));
            
            pixel.add<mode>(delta); // TODO: skip view_position, use z alone
        }

        left.add<mode>(scene_context.num_threads, *left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *right_edge_delta[triangle_half]);
    }
}

void triangle::render_textured_gouraud_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = textured_gouraud;
    
    if (halftriangle_height[triangle_half] <= 0)
        return;

    surface_position left = *left_edge_top[triangle_half];
    surface_position right = *right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);
    
    int y_skip = compute_y_skip(y, thread_index, scene_context.num_threads);

    if (y_skip != 0) {
        left.add<mode>(y_skip, *left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *right_edge_delta[triangle_half]);
        y += y_skip;
    }

    for (int step = static_cast<int>(scene_context.num_threads); y <= max_y; y += step) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta<mode>(left, right, max_x - x);
        max_x = std::min(max_x, scene_context.target->pixel_width() - 1);

        if (x < 0) {
            pixel.add<mode>(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            if (scene_context.target->depth_at(x, y) < pixel.view_position.z())
                scene_context.target->set_pixel_overwriting_z_buffer(x, y, pixel.view_position.z(),
                                                                     pixel.shade * mat->diffuse_texture_map(pixel.uv));
            
            pixel.add<mode>(delta); // TODO: skip view_position, use z alone
        }

        left.add<mode>(scene_context.num_threads, *left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *right_edge_delta[triangle_half]);
    }
}

void triangle::render_textured_smooth_phong_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = textured_smooth_phong;
    
    if (halftriangle_height[triangle_half] <= 0)
        return;

    surface_position left = *left_edge_top[triangle_half];
    surface_position right = *right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);

    int y_skip = compute_y_skip(y, thread_index, scene_context.num_threads);

    if (y_skip != 0) {
        left.add<mode>(y_skip, *left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *right_edge_delta[triangle_half]);
        y += y_skip;
    }

    for (int step = static_cast<int>(scene_context.num_threads); y <= max_y; y += step) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta<mode>(left, right, max_x - x);
        max_x = std::min(max_x, scene_context.target->pixel_width() - 1);

        if (x < 0) {
            pixel.add<mode>(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            if (scene_context.target->depth_at(x, y) < pixel.view_position.z())
                scene_context.target->set_pixel_overwriting_z_buffer(x, y, pixel.view_position.z(),
                                                                     mat->shade(pixel.world_position,
                                                                                pixel.normal.unit(),
                                                                                (scene_context.eye - pixel.world_position).unit(),
                                                                                pixel.uv,
                                                                                *scene_context.light_sources));

            pixel.add<mode>(delta); // TODO: skip view_position, use z alone
        }

        left.add<mode>(scene_context.num_threads, *left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *right_edge_delta[triangle_half]);
    }
}
#include <iostream>
void triangle::render_textured_flat_phong_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = textured_flat_phong;
    
    if (halftriangle_height[triangle_half] <= 0)
        return;

    surface_position left = *left_edge_top[triangle_half];
    surface_position right = *right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);

    int y_skip = compute_y_skip(y, thread_index, scene_context.num_threads);
    
    if (y_skip != 0) {
        left.add<mode>(y_skip, *left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *right_edge_delta[triangle_half]);
        y += y_skip;
    }

    for (int step = static_cast<int>(scene_context.num_threads); y <= max_y; y += step) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta<mode>(left, right, max_x - x);
        max_x = std::min(max_x, scene_context.target->pixel_width() - 1);

        if (x < 0) {
            pixel.add<mode>(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            if (scene_context.target->depth_at(x, y) < pixel.view_position.z())         
                scene_context.target->set_pixel_overwriting_z_buffer(x, y, pixel.view_position.z(),
                                                                     mat->shade(pixel.world_position,
                                                                                surface_normal,
                                                                                (scene_context.eye - pixel.world_position).unit(),
                                                                                pixel.uv,
                                                                                *scene_context.light_sources));

            pixel.add<mode>(delta); // TODO: skip view_position, use z alone
        }

        left.add<mode>(scene_context.num_threads, *left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *right_edge_delta[triangle_half]);
    }
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

shading_model_t triangle::compute_shading_limit() {
    illumination_model_t illum = mat->get_illumination_model();

    if (illum == constant_color)
        return flat;
    else
        return phong;
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
        short_edge_delta[0].compute_delta<full>(vertex[0], vertex[1], top_half_height);

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
        float bot_half_height = halftriangle_height[1];
        short_edge_delta[1].compute_delta<full>(vertex[1], vertex[2], bot_half_height);

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

int triangle::compute_y_skip(int y, size_t thread_index, size_t num_threads) const {
    if (y < 0)
        return thread_index - y;
    else
        return ((thread_index - (y % num_threads)) + num_threads) % num_threads;
}
