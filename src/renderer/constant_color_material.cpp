#include "constant_color_material.h"

constant_color_material::constant_color_material() :
    material(constant_color) {
}

constant_color_material::~constant_color_material() {
}

color constant_color_material::shade_flat(const math::vector3f&, const math::vector3f&, const light_list&) const {
    return get_diffuse_reflectivity();
}

color constant_color_material::shade_phong(const math::vector3f&, const math::vector3f&,
                                           const math::vector3f&, const light_list&) const {
    return get_diffuse_reflectivity();
}
