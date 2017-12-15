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

material *material::create(int illum) {
    if (illum == 0)
        return new constant_color_material();
    else if (illum == 1)
        return new diffuse_material();
    else if (illum == 2)
        return new specular_material();
    else
        throw unsupported_material_exception();
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

const color &material::get_transmission_filter() const {
    return transmission_filter;
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

void material::set_texture_map(const texture *t) {
    tex = t;
}
