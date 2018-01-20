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

color4 diffuse_material::shade_pixel(const surface_position &point, const scene_render_context &scene) const {
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
    color3 result(ambient_multiplier * scene.light_sources->ambient_coeff() +
        diffuse_multiplier * diffuse_sum +
        emissive_term);
    result.clamp();
    return color4(result, get_dissolve());
}

color3 diffuse_material::shade_vertex(const surface_position &point, const scene_render_context &scene) const {
    color3 diffuse_sum;

    for (const light *source : scene.light_sources->get()) {
        vector3f light_vector = source->surface_to_light_unit(point.world_position);
        float normal_dot_light(point.normal.unit().dot(light_vector));

        if (normal_dot_light > 0.0f)
            diffuse_sum += source->diffuse() * normal_dot_light; // TODO FMA
    }

    return diffuse_sum;
}

color4 diffuse_material::interpolate(const surface_position &point, const scene_render_context &scene) const {
    color3 ambient_multiplier = ambient_color_at(point.uv);
    color3 diffuse_multiplier = diffuse_color_at(point.uv);
    color3 emissive_term = emissive_color_at(point.uv);

    // TODO FMA
    color3 result(ambient_multiplier * scene.light_sources->ambient_coeff() +
                  point.shade * diffuse_multiplier +
                  emissive_term);
    result.clamp();
    return color4(result, get_dissolve());
}
