#ifndef DIFFUSE_MATERIAL_H
#define DIFFUSE_MATERIAL_H

#include "material.h"

// material.mtl: illum = 1
class diffuse_material : public material {
public:
    diffuse_material();
    ~diffuse_material();

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
