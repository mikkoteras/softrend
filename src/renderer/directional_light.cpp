#include "directional_light.h"

using namespace math;

directional_light::directional_light(const vector3f &direction, const color &light_color) :
    direction_to_light(-direction.unit()), // argument is direction from light
    diffuse_color(light_color),
    specular_color(light_color) {
}

directional_light::directional_light(const vector3f &direction,
                                     const color &diffuse_color, const color &specular_color) :
    direction_to_light(-direction.unit()), // argument is direction from light
    diffuse_color(diffuse_color),
    specular_color(specular_color) {
}

directional_light::~directional_light() {
}

directional_light::directional_light(const directional_light &rhs) :
    direction_to_light(rhs.direction_to_light),
    diffuse_color(rhs.diffuse_color),
    specular_color(rhs.specular_color) {
}

directional_light::directional_light(directional_light &&rhs) :
    direction_to_light(rhs.direction_to_light),
    diffuse_color(rhs.diffuse_color),
    specular_color(rhs.specular_color) {
}

const directional_light &directional_light::operator=(const directional_light &rhs) {
    direction_to_light = rhs.direction_to_light;
    diffuse_color = rhs.diffuse_color;
    specular_color = rhs.specular_color;
    return *this;
}

directional_light &directional_light::operator=(directional_light &&rhs) {
    direction_to_light = rhs.direction_to_light;
    diffuse_color = rhs.diffuse_color;
    specular_color = rhs.specular_color;
    return *this;
}

const vector3f &directional_light::light_vector_unit() const {
    return direction_to_light;
}

color directional_light::diffuse() const {
    return diffuse_color;
}

color directional_light::specular() const {
    return specular_color;
}
