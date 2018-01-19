#include "framebuffer.h"
#include "material.h"
#include "scene_render_context.h"

template<combined_interpolation_mode_t mode>
void triangle::render_triangle(const scene_render_context &scene_context, unsigned thread_index) const {
    if (scene_context.parent_scene->get_shading_model() == phong) {
        render_halftriangle<mode>(scene_context, thread_index, 0);
        render_halftriangle<mode>(scene_context, thread_index, 1);
    }
}

template<combined_interpolation_mode_t mode>
void triangle::render_halftriangle(const scene_render_context &scene_context,
                                   unsigned thread_index, int triangle_half) const {
    if (halftriangle_height[triangle_half] <= 0)
        return;

    bool fast_shading = scene_context.parent_scene->get_shading_model() != phong;

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
            if (scene_context.target->depth_at(x, y) < pixel.view_position.z()) {
                color4 shade = mat->shade(pixel, scene_context, fast_shading);
                scene_context.target->set_pixel_overwriting_z_buffer(x, y, pixel.view_position.z(), shade);
            }

            pixel.add<mode>(delta); // TODO: skip view_position, use z alone
        }

        left.add<mode>(scene_context.num_threads, *left_edge_delta[triangle_half]);
        right.add<mode>(scene_context.num_threads, *right_edge_delta[triangle_half]);
    }
}
