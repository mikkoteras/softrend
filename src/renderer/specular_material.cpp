#include "specular_material.h"
#include "color3.h"
#include "light.h"
#include "light_list.h"
#include "scene_render_context.h"
#include "surface_position.h"
#include "types.h"
#include "math/vector.h"

using namespace math;

specular_material::specular_material() :
    material(specular) {
}

specular_material::~specular_material() {
}

color4 specular_material::shade_pixel(const surface_position &point, const scene_render_context &scene) const {
    color3 ambient_multiplier = ambient_color_at(point.uv);
    color3 diffuse_multiplier = diffuse_color_at(point.uv);
    color3 specular_multiplier = specular_color_at(point.uv);
    color3 emissive_term = emissive_color_at(point.uv);
    color3 diffuse_sum, specular_sum;

    for (const light *source : scene.light_sources->get()) {
        vector3f light_vector = source->surface_to_light_unit(point.world_position);
        vector3f surface_normal_unit = point.normal.unit();
        float normal_dot_light(surface_normal_unit.dot(light_vector));

        if (normal_dot_light > 0.0f)
            diffuse_sum += source->diffuse() * normal_dot_light; // TODO FMA

        vector3f reflection_vector(2.0f * normal_dot_light * surface_normal_unit - light_vector);
        reflection_vector.normalize();
        vector3f point_to_eye_unit(scene.eye - point.world_position);
        point_to_eye_unit.normalize();
        float specular_base = point_to_eye_unit.dot(reflection_vector);

        if (specular_base > 0.0f)
            specular_sum += powf(specular_base, get_specular_exponent()) * source->specular();
    }

    // TODO FMA
    color3 result(ambient_multiplier * scene.light_sources->ambient_coeff() +
                  diffuse_multiplier * diffuse_sum +
                  specular_multiplier * specular_sum +
                  emissive_term);

    result.clamp();
    return color4(result, get_dissolve());
}

color3 specular_material::shade_vertex(const surface_position &point, const scene_render_context &scene) const {
    color3 diffuse_sum, specular_sum;

    for (const light *source : scene.light_sources->get()) {
        vector3f light_vector = source->surface_to_light_unit(point.world_position);
        vector3f surface_normal_unit = point.normal.unit();
        float normal_dot_light(surface_normal_unit.dot(light_vector));

        if (normal_dot_light > 0.0f)
            diffuse_sum += source->diffuse() * normal_dot_light; // TODO FMA

        vector3f reflection_vector(2.0f * normal_dot_light * surface_normal_unit - light_vector);
        reflection_vector.normalize();
        vector3f point_to_eye_unit(scene.eye - point.world_position);
        point_to_eye_unit.normalize();
        float specular_base = point_to_eye_unit.dot(reflection_vector);

        if (specular_base > 0.0f)
            specular_sum += powf(specular_base, get_specular_exponent()) * source->specular();
    }

    // TODO: two shade variables would actually be needed here for accurate results
    return (diffuse_sum + specular_sum) / 2.0f;
}

color4 specular_material::interpolate(const surface_position &point, const scene_render_context &scene) const {
    color3 ambient_multiplier = ambient_color_at(point.uv);
    color3 diffuse_multiplier = diffuse_color_at(point.uv);
    color3 specular_multiplier = specular_color_at(point.uv);
    color3 emissive_term = emissive_color_at(point.uv);

    // TODO FMA
    // TODO: two shade variables would actually be needed here for accurate results
    color3 result(ambient_multiplier * scene.light_sources->ambient_coeff() +
                  point.shade * (diffuse_multiplier + specular_multiplier) +
                  emissive_term);
    result.clamp();
    return color4(result, get_dissolve());
}
