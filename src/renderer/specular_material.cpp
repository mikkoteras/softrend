#include "specular_material.h"
#include "light.h"
#include "light_list.h"
#include "vector.h"

using namespace math;

specular_material::specular_material() :
    material(2) {
}

specular_material::~specular_material() {
}

color specular_material::shade_flat(const math::vector3f &surface_midpoint, const math::vector3f &surface_normal_unit,
                                   const light_list &light_sources) const {
    color result(get_ambient_reflectivity() * light_sources.ambient_coeff());
    
    for (const light *source: light_sources.get()) {
        vector3f light_vector = source->surface_to_light_unit(surface_midpoint);
        float normal_dot_light(surface_normal_unit.dot(light_vector));

        if (normal_dot_light > 0.0f)
            result += source->diffuse() * normal_dot_light;
    }

    result.clamp();
    return result;
}

color specular_material::shade_phong(const math::vector3f &surface_point, const math::vector3f &surface_normal_unit,
                                    const math::vector3f &point_to_eye_unit, const light_list &light_sources) const {
    color result(get_ambient_reflectivity() * light_sources.ambient_coeff());
    color diffuse_term, specular_term;

    for (const light *source: light_sources.get()) {
        vector3f light_vector = source->surface_to_light_unit(surface_point);
        float normal_dot_light(surface_normal_unit.dot(light_vector));

        if (normal_dot_light > 0.0f)
            specular_term += source->diffuse() * normal_dot_light;

        vector3f reflection_vector(2.0f * normal_dot_light * surface_normal_unit - light_vector);
        reflection_vector.normalize();
        float specular_base = point_to_eye_unit.dot(reflection_vector);

        if (specular_base > 0.0f)
            specular_term += powf(specular_base, get_specular_exponent()) * source->specular();
    }
    
    result += get_diffuse_reflectivity() * diffuse_term + get_specular_reflectivity() * specular_term;
    result.clamp();
    return result;
}
