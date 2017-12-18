#include "diffuse_material.h"
#include "light.h"
#include "light_list.h"
#include "types.h"
#include "vector.h"

using namespace math;

diffuse_material::diffuse_material() :
    material(diffuse) {
}

diffuse_material::~diffuse_material() {
}

color diffuse_material::shade_flat(const math::vector3f &surface_midpoint, const math::vector3f &surface_normal_unit,
                                   const light_list &light_sources) const {
    color result(get_ambient_reflectivity() * light_sources.ambient_coeff());
    
    for (const light *source: light_sources.get()) {
        vector3f light_vector = source->surface_to_light_unit(surface_midpoint);
        float normal_dot_light(surface_normal_unit.dot(light_vector));

        if (normal_dot_light > 0.0f)
            result += source->diffuse() * normal_dot_light;
    }

    result.alpha() = get_dissolve();
    result.clamp();
    return result;
}

color diffuse_material::shade_phong(const math::vector3f &surface_point, const math::vector3f &surface_normal_unit,
                                    const math::vector3f &point_to_eye_unit, const light_list &light_sources) const {
    color result(get_ambient_reflectivity() * light_sources.ambient_coeff());
    color diffuse_term;

    for (const light *source: light_sources.get()) {
        vector3f light_vector = source->surface_to_light_unit(surface_point);
        float normal_dot_light(surface_normal_unit.dot(light_vector));

        if (normal_dot_light > 0.0f)
            diffuse_term += source->diffuse() * normal_dot_light;
    }
    
    result += get_diffuse_reflectivity() * diffuse_term;
    result.alpha() = get_dissolve();
    result.clamp();
    return result;
}
