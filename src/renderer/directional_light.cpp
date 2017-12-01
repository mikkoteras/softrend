#include "directional_light.h"

using namespace math;

directional_light::directional_light(const vector3f &direction, const color &light_color) :
    light_direction(direction.unit()),
    light_color(light_color) {
}

directional_light::~directional_light() {
}

directional_light::directional_light(const directional_light &rhs) :
    light_direction(rhs.light_direction),
    light_color(rhs.light_color) {
}

directional_light::directional_light(directional_light &&rhs) :
    light_direction(rhs.light_direction),
    light_color(rhs.light_color) {
}

const directional_light &directional_light::operator=(const directional_light &rhs) {
    light_direction = rhs.light_direction;
    light_color = rhs.light_color;
    return *this;
}

directional_light &directional_light::operator=(directional_light &&rhs) {
    light_direction = rhs.light_direction;
    light_color = rhs.light_color;
    return *this;
}

vector3f directional_light::light_to_surface_unit(const vector3f&) const {
    return light_direction;
}

vector3f directional_light::surface_to_light_unit(const vector3f&) const {
    return -light_direction;
}

color directional_light::get_color() const {
    return light_color;
}
