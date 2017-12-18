#include "constant_color_material.h"

constant_color_material::constant_color_material() :
    material(constant_color) {
}

constant_color_material::~constant_color_material() {
}

color constant_color_material::shade_flat(const math::vector3f&, const math::vector3f&, const light_list&) const {
    color result = get_diffuse_reflectivity();
    result.alpha() = get_dissolve();
    result.clamp();
    return result;
}

color constant_color_material::shade_phong(const math::vector3f&, const math::vector3f&,
                                           const math::vector3f&, const light_list&) const {
    color result = get_diffuse_reflectivity();
    result.alpha() = get_dissolve();
    result.clamp();
    return result;
}
