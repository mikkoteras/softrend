#include "material.h"
#include "constant_color_material.h"
#include "diffuse_material.h"
#include "light.h"
#include "light_list.h"
#include "specular_material.h"
#include "texture.h"
#include "vector.h"

using namespace math;

material::material(illumination_model_t illum) :
    illumination_model(illum) {
}

material::~material() {
}

material *material::create(illumination_model_t illum) {
    if (illum == constant_color)
        return new constant_color_material();
    else if (illum == diffuse)
        return new diffuse_material();
    else if (illum == specular)
        return new specular_material();
    else
        throw unsupported_material_exception();
}

color material::diffuse_texture_map(const math::vector2f &uv) const {
    color result;
    
    if (diffuse_map)
        result = diffuse_map->at(uv);
    else
        result = color(1.0f, 1.0f, 1.0f, 1.0f);

    result.alpha() = get_dissolve();
    return result;
}

bool material::is_textured() const {
    return ambient_map || diffuse_map || specular_map || emissive_map;
}

bool material::is_colored() const {
    return !is_textured();
}

illumination_model_t material::get_illumination_model() const {
    return illumination_model;
}

const color &material::get_ambient_reflectivity() const {
    return ambient_reflectivity;
}

const color &material::get_diffuse_reflectivity() const {
    return diffuse_reflectivity;
}

const color &material::get_specular_reflectivity() const {
    return specular_reflectivity;
}

const color &material::get_emissivity() const {
    return emissivity;
}

float material::get_specular_exponent() const {
    return specular_exponent;
}

float material::get_dissolve() const {
    return dissolve;
}

bool material::get_dissolve_halo() const {
    return dissolve_halo;
}

const texture *material::get_ambient_map() const {
    return ambient_map;
}

const texture *material::get_diffuse_map() const {
    return diffuse_map;
}

const texture *material::get_specular_map() const {
    return specular_map;
}

const texture *material::get_emissive_map() const {
    return emissive_map;
}

void material::set_ambient_reflectivity(const color &col) {
    ambient_reflectivity = col;
}


void material::set_diffuse_reflectivity(const color &col) {
    diffuse_reflectivity = col;
}

void material::set_specular_reflectivity(const color &col) {
    specular_reflectivity = col;
}

void material::set_emissivity(const color &col) {
    emissivity = col;
}

void material::set_dissolve(float value, bool halo) {
    dissolve = value;
    dissolve_halo = halo;
}

void material::set_specular_exponent(float value) {
    specular_exponent = value;
}

void material::set_ambient_map(const texture *t) {
    ambient_map = t;
}

void material::set_diffuse_map(const texture *t) {
    diffuse_map = t;
}

void material::set_specular_map(const texture *t) {
    specular_map = t;
}

void material::set_emissive_map(const texture *t) {
    emissive_map = t;
}

color material::ambient_color_at(const vector2f &uv) const {
    if (ambient_map)
        return ambient_reflectivity * ambient_map->at(uv);
    else
        return ambient_reflectivity;
}

color material::diffuse_color_at(const vector2f &uv) const {
    if (diffuse_map)
        return diffuse_reflectivity * diffuse_map->at(uv);
    else
        return diffuse_reflectivity;
}

color material::specular_color_at(const vector2f &uv) const {
    if (specular_map)
        return specular_reflectivity * specular_map->at(uv);
    else
        return specular_reflectivity;
}

color material::emissive_color_at(const vector2f &uv) const {
    if (emissive_map)
        return emissivity * emissive_map->at(uv);
    else
        return emissivity;
}
