#include "point_light.h"

using namespace math;

point_light::point_light(const vector3f &position, const color3 &light_color) :
    position(position),
    diffuse_color(light_color),
    specular_color(light_color) {
}

point_light::point_light(const vector3f &position, const color3 &diffuse_color, const color3 &specular_color) :
    position(position),
    diffuse_color(diffuse_color),
    specular_color(specular_color) {
}

point_light::~point_light() {
}

point_light::point_light(const point_light &rhs) :
    position(rhs.position),
    diffuse_color(rhs.diffuse_color),
    specular_color(rhs.specular_color) {
}

point_light::point_light(point_light &&rhs) :
    position(rhs.position),
    diffuse_color(rhs.diffuse_color),
    specular_color(rhs.specular_color) {
}

const point_light &point_light::operator=(const point_light &rhs) {
    position = rhs.position;
    diffuse_color = rhs.diffuse_color;
    specular_color = rhs.specular_color;
    return *this;
}

point_light &point_light::operator=(point_light &&rhs) {
    position = rhs.position;
    diffuse_color = rhs.diffuse_color;
    specular_color = rhs.specular_color;
    return *this;
}

vector3f point_light::light_to_surface_unit(const vector3f &surface_point) const {
    vector3f result = surface_point;
    result -= position;
    result.normalize();
    return result;
}

vector3f point_light::surface_to_light_unit(const vector3f &surface_point) const {
    vector3f result = position;
    result -= surface_point;
    result.normalize();
    return result;
}

color3 point_light::diffuse() const {
    return diffuse_color;
}

color3 point_light::specular() const {
    return specular_color;
}
