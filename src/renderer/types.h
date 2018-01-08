#ifndef SOFTREND_TYPES_H
#define SOFTREND_TYPES_H

enum shading_model_t {
    flat = 0,
    gouraud,
    phong,
    max_shading_model = phong
};

enum illumination_model_t {
    constant_color = 0,
    diffuse = 1,
    specular = 2
};

enum interpolation_mode_t {
    interpolate_view_position  = 1 << 0,
    interpolate_world_position = 1 << 1,
    interpolate_normal         = 1 << 2,
    interpolate_uv             = 1 << 3,
    interpolate_shade          = 1 << 4
};

enum combined_interpolation_mode_t {
    colored_flat          = interpolate_view_position,
    colored_gouraud       = interpolate_view_position | interpolate_shade,
    colored_flat_phong    = interpolate_view_position | interpolate_world_position,
    colored_smooth_phong  = interpolate_view_position | interpolate_world_position | interpolate_normal,
    textured_flat         = interpolate_view_position | interpolate_uv,
    textured_gouraud      = interpolate_view_position | interpolate_uv | interpolate_shade,
    textured_flat_phong   = interpolate_view_position | interpolate_world_position | interpolate_uv,
    textured_smooth_phong = interpolate_view_position | interpolate_world_position | interpolate_normal | interpolate_uv,
    full                  = interpolate_view_position | interpolate_world_position | interpolate_normal |
                            interpolate_uv | interpolate_uv
};

#endif
