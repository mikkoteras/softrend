#include "constant_color_material.h"
#include "scene_render_context.h"
#include "surface_position.h"

constant_color_material::constant_color_material() :
    material(constant_color) {
}

constant_color_material::~constant_color_material() {
}

color4 constant_color_material::shade_pixel(const surface_position &point, const scene_render_context&) const {
    return color4(diffuse_color_at(point.uv), get_dissolve());
}

color3 constant_color_material::shade_vertex(const surface_position &point, const scene_render_context&) const {
    return diffuse_color_at(point.uv);
}

color4 constant_color_material::interpolate(const surface_position &point, const scene_render_context&) const {
    return color4(diffuse_color_at(point.uv), get_dissolve());
}
