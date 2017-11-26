#include "material.h"
#include "texture.h"
#include "vector.h"

using namespace math;

material::material() :
    ambient_reflectivity{0, 0, 0},
    diffuse_reflectivity{0, 0, 0},
    specular_reflectivity{0, 0, 0},
    dissolve(0.0f),
    dissolve_halo(false),
    specular_exponent(0.0f),
    sharpness(60.0),
    illumination_model(-1),
    tex(nullptr) {
}

material::~material() {
}

color material::shade(const light_list &lights, const vector3f &surface_normal) const {
    
}

const vector3f &material::get_ambient_reflectivity() const {
    return ambient_reflectivity;
}

void material::set_ambient_reflectivity(const vector3f &vec) {
    ambient_reflectivity = vec;
}

const vector3f &material::get_diffuse_reflectivity() const {
    return diffuse_reflectivity;
}

void material::set_diffuse_reflectivity(const vector3f &vec) {
    diffuse_reflectivity = vec;
}

const vector3f &material::get_specular_reflectivity() const {
    return specular_reflectivity;
}

void material::set_specular_reflectivity(const vector3f &vec) {
    specular_reflectivity = vec;
}

const vector3f &material::get_transmission_filter() const {
    return transmission_filter;
}

void material::set_transmission_filter(const vector3f &vec) {
    transmission_filter = vec;
}

float material::get_dissolve() const {
    return dissolve;
}

bool material::get_dissolve_halo() const {
    return dissolve_halo;
}

void material::set_dissolve(float value, bool halo) {
    dissolve = value;
    dissolve_halo = halo;
}

float material::get_specular_exponent() const {
    return specular_exponent;
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

int material::get_illumination_model() const {
    return illumination_model;
}

void material::set_illumination_model(int value) {
    illumination_model = value;
}

const texture *material::get_texture_map() const {
    return tex;
}

void material::set_texture_map(const texture *t) {
    tex = t;
}
