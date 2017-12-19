#ifndef CONSTANT_COLOR_MATERIAL_H
#define CONSTANT_COLOR_MATERIAL_H

#include "material.h"

// mtl: illum = 0
class constant_color_material : public material {
public:
    constant_color_material();
    ~constant_color_material();

    color shade_flat(const math::vector3f &surface_midpoint, const math::vector3f &surface_normal_unit,
                     const light_list &light_sources) const override;
    color shade_phong(const math::vector3f &surface_point, const math::vector3f &surface_normal_unit,
                      const math::vector3f &point_to_eye_unit, const light_list &light_sources) const override;
};

#endif