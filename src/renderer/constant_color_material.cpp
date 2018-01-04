#include "constant_color_material.h"

constant_color_material::constant_color_material() :
    material(constant_color) {
}

constant_color_material::~constant_color_material() {
}

color4 constant_color_material::shade(const math::vector3f&,
                                      const math::vector3f&,
                                      const math::vector3f&,
                                      const math::vector2f &uv_coordinates,
                                      const light_list&) const {
    return color4(diffuse_color_at(uv_coordinates), get_dissolve());
}

color4 constant_color_material::shade(const math::vector3f&,
                                      const math::vector3f&,
                                      const math::vector3f&,
                                      const light_list&) const {
    return color4(get_diffuse_reflectivity(), get_dissolve());
}
