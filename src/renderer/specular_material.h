#ifndef SPECULAR_MATERIAL_H
#define SPECULAR_MATERIAL_H

#include "material.h"
#include "color4.h"

// material.mtl: illum = 2
class specular_material : public material {
public:
    specular_material();
    ~specular_material();

    color4 shade(const math::vector3f &surface_point,
                 const math::vector3f &surface_normal_unit,
                 const math::vector3f &point_to_eye_unit,
                 const math::vector2f &uv_coordinates,
                 const light_list &light_sources) const override;
    color4 shade(const math::vector3f &surface_point,
                 const math::vector3f &surface_normal_unit,
                 const math::vector3f &point_to_eye_unit,
                 const light_list &light_sources) const override;
};

#endif
