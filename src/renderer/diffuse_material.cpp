#include "diffuse_material.h"
#include "color3.h"
#include "light.h"
#include "light_list.h"
#include "scene_render_context.h"
#include "surface_position.h"
#include "types.h"
#include "math/vector.h"

using namespace math;

diffuse_material::diffuse_material() :
    material(diffuse) {
}

diffuse_material::~diffuse_material() {
}

color4 diffuse_material::shade(const surface_position &point, const scene_render_context &scene) const {
    color3 ambient_multiplier = ambient_color_at(point.uv);
    color3 diffuse_multiplier = diffuse_color_at(point.uv);
    color3 emissive_term = emissive_color_at(point.uv);
    color3 diffuse_sum;

    for (const light *source : scene.light_sources->get()) {
        vector3f light_vector = source->surface_to_light_unit(point.world_position);
        float normal_dot_light(point.normal.unit().dot(light_vector));

        if (normal_dot_light > 0.0f)
            diffuse_sum += source->diffuse() * normal_dot_light; // TODO FMA
    }

    // TODO FMA
    color3 result(ambient_multiplier * scene.light_sources->ambient_coeff() + // TODO FMA
        diffuse_multiplier * diffuse_sum +
        emissive_term);
    result.clamp();
    return color4(result, get_dissolve());
}

color4 diffuse_material::shade(const vector3f &surface_point,
                               const vector3f &surface_normal_unit,
                               const vector3f &point_to_eye_unit,
                               const vector2f &uv_coordinates,
                               const light_list &light_sources) const {
    color3 ambient_multiplier = ambient_color_at(uv_coordinates);
    color3 diffuse_multiplier = diffuse_color_at(uv_coordinates);
    color3 emissive_term = emissive_color_at(uv_coordinates);

    color3 diffuse_sum;
    
    for (const light *source: light_sources.get()) {
        vector3f light_vector = source->surface_to_light_unit(surface_point);
        float normal_dot_light(surface_normal_unit.dot(light_vector));

        if (normal_dot_light > 0.0f)
            diffuse_sum += source->diffuse() * normal_dot_light; // TODO FMA
    }

    // TODO FMA
    color3 result(ambient_multiplier * light_sources.ambient_coeff() + // TODO FMA
                  diffuse_multiplier * diffuse_sum +
                  emissive_term);
    result.clamp();
    return color4(result, get_dissolve());
}

color4 diffuse_material::shade(const vector3f &surface_point,
                              const vector3f &surface_normal_unit,
                              const vector3f &point_to_eye_unit,
                              const light_list &light_sources) const {
    color3 diffuse_sum;
    
    for (const light *source: light_sources.get()) {
        vector3f light_vector = source->surface_to_light_unit(surface_point);
        float normal_dot_light(surface_normal_unit.dot(light_vector));

        if (normal_dot_light > 0.0f)
            diffuse_sum += source->diffuse() * normal_dot_light; // TODO FMA
    }

    // TODO FMA
    color3 result(get_ambient_reflectivity() * light_sources.ambient_coeff() + // TODO FMA
                 get_diffuse_reflectivity() * diffuse_sum +
                 get_emissivity());
    result.clamp();
    return color4(result, get_dissolve());
}
