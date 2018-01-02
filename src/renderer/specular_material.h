#ifndef SPECULAR_MATERIAL_H
#define SPECULAR_MATERIAL_H

#include "material.h"

// material.mtl: illum = 2
class specular_material : public material {
public:
    specular_material();
    ~specular_material();

    color shade(const math::vector3f &surface_point,
                const math::vector3f &surface_normal_unit,
                const math::vector3f &point_to_eye_unit,
                const math::vector2f &uv_coordinates,
                const light_list &light_sources) const override;
    color shade(const math::vector3f &surface_point,
                const math::vector3f &surface_normal_unit,
                const math::vector3f &point_to_eye_unit,
                const light_list &light_sources) const override;
};

#endif
