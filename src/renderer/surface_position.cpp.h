// This depends on the compiler being able to cut off the statement if (false) ...;
// If this won't work, template metamagic might be tried.

#include "types.h"

template<unsigned interpolation_mode>
void surface_position::add(const surface_position &delta) {
    
    
    if (interpolation_mode & interpolate_view_position)
        view_position += delta.view_position;

    if (interpolation_mode & interpolate_world_position)
        world_position += delta.world_position;

    if (interpolation_mode & interpolate_normal)
        normal += delta.normal;

    if (interpolation_mode & interpolate_uv)
        uv += delta.uv;

    if (interpolation_mode & interpolate_shade)
        shade += delta.shade;
}

template<unsigned interpolation_mode>
void surface_position::add(float multiplier, const surface_position &delta) {
    if (interpolation_mode & interpolate_view_position)
        view_position += multiplier * delta.view_position;

    if (interpolation_mode & interpolate_world_position)
        world_position += multiplier * delta.world_position;

    if (interpolation_mode & interpolate_normal)
        normal += multiplier * delta.normal;

    if (interpolation_mode & interpolate_uv)
        uv += multiplier * delta.uv;

    if (interpolation_mode & interpolate_shade)
        shade += multiplier * delta.shade;
}

template<unsigned interpolation_mode>
void surface_position::compute_delta(const surface_position &v1, const surface_position &v2, float steps) {
    if (interpolation_mode & interpolate_view_position)
        view_position = (v2.view_position - v1.view_position) / steps;

    if (interpolation_mode & interpolate_world_position)
        world_position = (v2.world_position - v1.world_position) / steps;

    if (interpolation_mode & interpolate_normal)
        normal = (v2.normal - v1.normal) / steps;

    if (interpolation_mode & interpolate_uv)
        uv = (v2.uv - v1.uv) / steps;

    if (interpolation_mode & interpolate_shade)
        shade = (v2.shade - v1.shade) / steps;
}
