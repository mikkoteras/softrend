#include "material.h"
#include "light.h"
#include "light_list.h"
#include "texture.h"
#include "vector.h"

using namespace math;

material::material() :
    dissolve(0.0f),
    dissolve_halo(false),
    specular_exponent(2.0f),
    sharpness(60.0),
    illumination_model(-1),
    tex(nullptr) {
}

material::~material() {
}

color material::shade(const vector3f &surface_normal_unit, const vector3f &eye_position,
                      const light_list &light_sources, const color &texture_color) const {
    color result = ambient_reflectivity * light_sources.ambient_coeff();
    color diffuse_term, specular_term;

    for (const light *source: light_sources.get()) {
        diffuse_term += source->diffuse() * source->light_vector_unit().dot(surface_normal_unit);
        vector3f r(2.0f * source->light_vector_unit().dot(surface_normal_unit) * surface_normal_unit -
                   source->light_vector_unit());
        float eye_angle_scalar = r.dot(-eye_position);

        if (eye_angle_scalar > 0.0f)
            specular_term += powf(eye_angle_scalar, specular_exponent) * source->specular();
    }

    diffuse_term.clamp();
    specular_term.clamp();
    result += diffuse_reflectivity * diffuse_term + specular_reflectivity * specular_term;
    return result * texture_color;
}

const color &material::get_ambient_reflectivity() const {
    return ambient_reflectivity;
}

const color &material::get_diffuse_reflectivity() const {    return diffuse_reflectivity;
}

const color &material::get_specular_reflectivity() const {
    return specular_reflectivity;
}

const color &material::get_transmission_filter() const {
    return transmission_filter;
}

float material::get_dissolve() const {
    return dissolve;
}

bool material::get_dissolve_halo() const {
    return dissolve_halo;
}

float material::get_specular_exponent() const {
    return specular_exponent;
}

int material::get_illumination_model() const {
    return illumination_model;
}

const texture *material::get_texture_map() const {
    return tex;
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


void material::set_transmission_filter(const color &col) {
    transmission_filter = col;
}

void material::set_dissolve(float value, bool halo) {
    dissolve = value;
    dissolve_halo = halo;
}


void material::set_specular_exponent(float value) {
    specular_exponent = value;
}

float material::get_sharpness() const {
    return sharpness;
}

void material::set_sharpness(float value) {
    sharpness = value;
}

float material::get_optical_density() const {
    return optical_density;
}

void material::set_optical_density(float value) {
    optical_density = value;
}

void material::set_illumination_model(int value) {
    illumination_model = value;
}

void material::set_texture_map(const texture *t) {
    tex = t;
}
