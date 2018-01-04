#include "directional_light.h"

using namespace math;

directional_light::directional_light(const vector3f &direction, const color3 &light_color) :
    light_direction(direction.unit()),
    diffuse_color(light_color),
    specular_color(light_color) {
}

directional_light::directional_light(const vector3f &direction,
                                     const color3 &diffuse_color, const color3 &specular_color) :
    light_direction(direction.unit()),
    diffuse_color(diffuse_color),
    specular_color(specular_color) {
}

directional_light::~directional_light() {
}

directional_light::directional_light(const directional_light &rhs) :
    light_direction(rhs.light_direction),
    diffuse_color(rhs.diffuse_color),
    specular_color(rhs.specular_color) {
}

directional_light::directional_light(directional_light &&rhs) :
    light_direction(rhs.light_direction),
    diffuse_color(rhs.diffuse_color),
    specular_color(rhs.specular_color) {
}

const directional_light &directional_light::operator=(const directional_light &rhs) {
    light_direction = rhs.light_direction;
    diffuse_color = rhs.diffuse_color;
    specular_color = rhs.specular_color;
    return *this;
}

directional_light &directional_light::operator=(directional_light &&rhs) {
    light_direction = rhs.light_direction;
    diffuse_color = rhs.diffuse_color;
    specular_color = rhs.specular_color;
    return *this;
}

vector3f directional_light::light_to_surface_unit(const vector3f&) const {
    return light_direction;
}

vector3f directional_light::surface_to_light_unit(const vector3f&) const {
    return -light_direction;
}

color3 directional_light::diffuse() const {
    return diffuse_color;
}

color3 directional_light::specular() const {
    return specular_color;
}
