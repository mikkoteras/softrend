#include "triangle.h"
#include "color3.h"
#include "framebuffer.h"
#include "light.h"
#include "light_list.h"
#include "material.h"
#include "scene.h"
#include "triangle_render_context.h"
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
    has_distinct_normals(ni1 != ni2 || ni1 != ni3),
    has_uv_coordinates(true),
    shading_limit(compute_shading_limit()) {
}

triangle::triangle(unsigned vi1, unsigned vi2, unsigned vi3, unsigned ni1, unsigned ni2, unsigned ni3,
                   const material *mat) :
    vertex_index{vi1, vi2, vi3},
    vertex_uv{vector2f(), vector2f(), vector2f()},
    normal_index{ni1, ni2, ni3},
    mat(mat),
    has_distinct_normals(ni1 != ni2 || ni1 != ni3),
    has_uv_coordinates(false),
    shading_limit(compute_shading_limit()) {
}

triangle::triangle(const triangle &rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
        normal_index[i] = rhs.normal_index[i];
    }

    mat = rhs.mat;
    has_distinct_normals = rhs.has_distinct_normals;
    has_uv_coordinates = rhs.has_uv_coordinates;
    shading_limit = rhs.shading_limit;
}

triangle::triangle(triangle &&rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
        normal_index[i] = rhs.normal_index[i];
    }

    mat = rhs.mat;
    has_distinct_normals = rhs.has_distinct_normals;
    has_uv_coordinates = rhs.has_uv_coordinates;
    shading_limit = rhs.shading_limit;
}

const triangle &triangle::operator=(const triangle &rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
        normal_index[i] = rhs.normal_index[i];
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
        vertex_uv[i] = rhs.vertex_uv[i];
        normal_index[i] = rhs.normal_index[i];
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
    
    for (int i = 0; i < 3; ++i)
        render_context.vtx(i).view_position = view_coord[vertex_index[i]];

    skip_render = triangle_winds_clockwise() ||
                  scene_context.parent_scene->visible_volume().clip(render_context.vtx(0).view_position,
                                                                    render_context.vtx(1).view_position,
                                                                    render_context.vtx(2).view_position);

    if (skip_render)
        return;

    const vector3f *world_coord = scene_context.parent_scene->world_coordinate_data();
    const vector3f *world_normal = scene_context.parent_scene->world_normal_data();
    render_context.surface_midpoint = vector3f{0.0f, 0.0f, 0.0f};

    for (int i = 0; i < 3; ++i) {
        surface_position &vtx = render_context.vtx(i);
        vtx.world_position = world_coord[vertex_index[i]];
        vtx.normal = world_normal[normal_index[i]];
        vtx.uv = vertex_uv[i];
        render_context.surface_midpoint += vtx.world_position;
    }

    render_context.surface_midpoint /= 3.0f;

    if (shading_limit >= gouraud && scene_context.parent_scene->get_shading_model() == gouraud)
        for (int i = 0; i < 3; ++i) {
            surface_position &vtx = render_context.vtx(i);
            vtx.shade = mat->shade(vtx.world_position,
                                   vtx.normal,
                                   (scene_context.eye - vtx.world_position).unit(),
                                   *scene_context.light_sources);
        }

    render_context.prepare_edges();
    render_context.prepare_upper_halftriangle();
    render_context.prepare_lower_halftriangle();
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

bool triangle::triangle_winds_clockwise() const {
    float x[3], y[3];
    
    for (int i = 0; i < 3; ++i) {
        const vector3f &v = render_context.vtx(i).view_position;
        x[i] = v.x();
        y[i] = v.y();
    }

    float sum = (x[1] - x[0]) * (y[1] + y[0]) +
                (x[2] - x[1]) * (y[2] + y[1]) +
                (x[0] - x[2]) * (y[0] + y[2]);
    
    return sum < 0.0f;
}

void triangle::render_colored_flat_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = colored_flat;

    if (render_context.halftriangle_height <= 0)
        return;

    surface_position left = *render_context.left_edge_top[triangle_half];
    surface_position right = *render_context.right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);

    int y_skip = render_context.compute_y_skip(y, thread_index, scene_context.num_threads);

    if (y_skip != 0) {
        left.add<mode>(y_skip, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *render_context.right_edge_delta[triangle_half]);
        y += y_skip;
    }

    color4 shade(mat->shade(render_context.surface_midpoint,
                            render_context.surface_normal,
                            (scene_context.eye - render_context.surface_midpoint).unit(),
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

        left.add<mode>(scene_context.num_threads, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *render_context.right_edge_delta[triangle_half]);
    }
}

void triangle::render_colored_gouraud_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = colored_gouraud;
    
    if (render_context.halftriangle_height <= 0)
        return;

    surface_position left = *render_context.left_edge_top[triangle_half];
    surface_position right = *render_context.right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);

    int y_skip = render_context.compute_y_skip(y, thread_index, scene_context.num_threads);

    if (y_skip != 0) {
        left.add<mode>(y_skip, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *render_context.right_edge_delta[triangle_half]);
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

        left.add<mode>(scene_context.num_threads, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *render_context.right_edge_delta[triangle_half]);
    }
}

void triangle::render_colored_smooth_phong_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = colored_smooth_phong;
    
    if (render_context.halftriangle_height <= 0)
        return;

    surface_position left = *render_context.left_edge_top[triangle_half];
    surface_position right = *render_context.right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);

    int y_skip = render_context.compute_y_skip(y, thread_index, scene_context.num_threads);

    if (y_skip != 0) {
        left.add<mode>(y_skip, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *render_context.right_edge_delta[triangle_half]);
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

        left.add<mode>(scene_context.num_threads, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *render_context.right_edge_delta[triangle_half]);
    }
}

void triangle::render_colored_flat_phong_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = colored_flat_phong;
    
    if (render_context.halftriangle_height <= 0)
        return;

    surface_position left = *render_context.left_edge_top[triangle_half];
    surface_position right = *render_context.right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);

    int y_skip = render_context.compute_y_skip(y, thread_index, scene_context.num_threads);

    if (y_skip != 0) {
        left.add<mode>(y_skip, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *render_context.right_edge_delta[triangle_half]);
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
                                                                                render_context.surface_normal,
                                                                                (scene_context.eye - pixel.world_position).unit(),
                                                                                pixel.uv,
                                                                                *scene_context.light_sources));

            pixel.add<mode>(delta); // TODO: skip view_position, use z alone
        }

        left.add<mode>(scene_context.num_threads, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *render_context.right_edge_delta[triangle_half]);
    }
}

void triangle::render_textured_flat_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = textured_flat;
    
    if (render_context.halftriangle_height <= 0)
        return;

    surface_position left = *render_context.left_edge_top[triangle_half];
    surface_position right = *render_context.right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);
    
    int y_skip = render_context.compute_y_skip(y, thread_index, scene_context.num_threads);

    if (y_skip != 0) {
        left.add<mode>(y_skip, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *render_context.right_edge_delta[triangle_half]);
        y += y_skip;
    }
    color4 shade(mat->shade(render_context.surface_midpoint,
                            render_context.surface_normal,
                            (scene_context.eye - render_context.surface_midpoint).unit(),
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

        left.add<mode>(scene_context.num_threads, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *render_context.right_edge_delta[triangle_half]);
    }
}

void triangle::render_textured_gouraud_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = textured_gouraud;
    
    if (render_context.halftriangle_height <= 0)
        return;

    surface_position left = *render_context.left_edge_top[triangle_half];
    surface_position right = *render_context.right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);
    
    int y_skip = render_context.compute_y_skip(y, thread_index, scene_context.num_threads);

    if (y_skip != 0) {
        left.add<mode>(y_skip, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *render_context.right_edge_delta[triangle_half]);
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

        left.add<mode>(scene_context.num_threads, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *render_context.right_edge_delta[triangle_half]);
    }
}

void triangle::render_textured_smooth_phong_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = textured_smooth_phong;
    
    if (render_context.halftriangle_height <= 0)
        return;

    surface_position left = *render_context.left_edge_top[triangle_half];
    surface_position right = *render_context.right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);

    int y_skip = render_context.compute_y_skip(y, thread_index, scene_context.num_threads);

    if (y_skip != 0) {
        left.add<mode>(y_skip, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *render_context.right_edge_delta[triangle_half]);
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

        left.add<mode>(scene_context.num_threads, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *render_context.right_edge_delta[triangle_half]);
    }
}

void triangle::render_textured_flat_phong_halftriangle(const scene_render_context &scene_context, unsigned thread_index, int triangle_half) const {
    static const combined_interpolation_mode_t mode = textured_flat_phong;
    
    if (render_context.halftriangle_height <= 0)
        return;

    surface_position left = *render_context.left_edge_top[triangle_half];
    surface_position right = *render_context.right_edge_top[triangle_half];
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height[triangle_half];
    max_y = std::min(max_y, scene_context.target->pixel_height() - 1);

    int y_skip = render_context.compute_y_skip(y, thread_index, scene_context.num_threads);
    
    if (y_skip != 0) {
        left.add<mode>(y_skip, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(y_skip, *render_context.right_edge_delta[triangle_half]);
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
                                                                                render_context.surface_normal,
                                                                                (scene_context.eye - pixel.world_position).unit(),
                                                                                pixel.uv,
                                                                                *scene_context.light_sources));

            pixel.add<mode>(delta); // TODO: skip view_position, use z alone
        }

        left.add<mode>(scene_context.num_threads, *render_context.left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *render_context.right_edge_delta[triangle_half]);
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
