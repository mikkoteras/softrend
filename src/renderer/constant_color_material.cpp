#include "constant_color_material.h"
#include "scene_render_context.h"
#include "surface_position.h"

constant_color_material::constant_color_material() :
    material(constant_color) {
}

constant_color_material::~constant_color_material() {
}

color4 constant_color_material::shade_flat(const surface_position &point, const scene_render_context &scene) const {
    return color4(diffuse_color_at(point.uv), get_dissolve());
}

color4 constant_color_material::shade_gouraud(const surface_position &point, const scene_render_context &scene) const {
    return color4(diffuse_color_at(point.uv), get_dissolve());
}

color4 constant_color_material::shade_phong(const surface_position &point, const scene_render_context &scene) const {
    return color4(diffuse_color_at(point.uv), get_dissolve());
}
