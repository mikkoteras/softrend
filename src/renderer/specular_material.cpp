#include "specular_material.h"
#include "light.h"
#include "light_list.h"
#include "types.h"
#include "vector.h"

using namespace math;

specular_material::specular_material() :
    material(specular) {
}

specular_material::~specular_material() {
}

color specular_material::shade(const math::vector3f &surface_point,
                               const math::vector3f &surface_normal_unit,
                               const math::vector3f &point_to_eye_unit,
                               const vector2f &uv_coordinates,
                               const light_list &light_sources) const {
    color ambient_multiplier = ambient_color_at(uv_coordinates);
    color diffuse_multiplier = diffuse_color_at(uv_coordinates);
    color specular_multiplier = specular_color_at(uv_coordinates);
    color emissive_term = emissive_color_at(uv_coordinates);
    
    color diffuse_sum, specular_sum;

    for (const light *source: light_sources.get()) {
        vector3f light_vector = source->surface_to_light_unit(surface_point);
        float normal_dot_light(surface_normal_unit.dot(light_vector));

        if (normal_dot_light > 0.0f)
            diffuse_sum += source->diffuse() * normal_dot_light; // TODO FMA

        vector3f reflection_vector(2.0f * normal_dot_light * surface_normal_unit - light_vector);
        reflection_vector.normalize();
        float specular_base = point_to_eye_unit.dot(reflection_vector);

        if (specular_base > 0.0f)
            specular_sum += powf(specular_base, get_specular_exponent()) * source->specular();
    }

    color result(ambient_multiplier * light_sources.ambient_coeff() + // TOOD FMA
                 diffuse_multiplier * diffuse_sum +
                 specular_multiplier * specular_sum +
                 emissive_term);
    result.alpha() = get_dissolve();
    result.clamp();
    return result;
}

color specular_material::shade(const math::vector3f &surface_point,
                               const math::vector3f &surface_normal_unit,
                               const math::vector3f &point_to_eye_unit,
                               const light_list &light_sources) const {
    color diffuse_sum, specular_sum;

    for (const light *source: light_sources.get()) {
        vector3f light_vector = source->surface_to_light_unit(surface_point);
        float normal_dot_light(surface_normal_unit.dot(light_vector));

        if (normal_dot_light > 0.0f)
            diffuse_sum += source->diffuse() * normal_dot_light; // TODO FMA

        vector3f reflection_vector(2.0f * normal_dot_light * surface_normal_unit - light_vector);
        reflection_vector.normalize();
        float specular_base = point_to_eye_unit.dot(reflection_vector);

        if (specular_base > 0.0f)
            specular_sum += powf(specular_base, get_specular_exponent()) * source->specular();
    }

    color result(get_ambient_reflectivity() * light_sources.ambient_coeff() + // TOOD FMA
                 get_diffuse_reflectivity() * diffuse_sum +
                 get_specular_reflectivity() * specular_sum +
                 get_emissivity());
    result.alpha() = get_dissolve();
    result.clamp();
    return result;
}
