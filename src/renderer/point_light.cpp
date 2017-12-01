#include "point_light.h"

using namespace math;

point_light::point_light(const vector3f &position, const color &light_color) :
    position(position),
    light_color(light_color) {
}

point_light::~point_light() {
}

point_light::point_light(const point_light &rhs) :
    position(rhs.position),
    light_color(rhs.light_color) {
}

point_light::point_light(point_light &&rhs) :
    position(rhs.position),
    light_color(rhs.light_color) {
}

const point_light &point_light::operator=(const point_light &rhs) {
    position = rhs.position;
    light_color = rhs.light_color;
    return *this;
}

point_light &point_light::operator=(point_light &&rhs) {
    position = rhs.position;
    light_color = rhs.light_color;
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

color point_light::get_color() const {
    return light_color;
}
