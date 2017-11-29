#include "directional_light.h"

using namespace math;

directional_light::directional_light(const vector3f &direction, const color &light_color) :
    direction_to_light(-direction.unit()), // argument is direction from light
    light_color(light_color) {
}

directional_light::~directional_light() {
}

directional_light::directional_light(const directional_light &rhs) :
    direction_to_light(rhs.direction_to_light),
    light_color(rhs.light_color) {
}

directional_light::directional_light(directional_light &&rhs) :
    direction_to_light(rhs.direction_to_light),
    light_color(rhs.light_color) {
}

const directional_light &directional_light::operator=(const directional_light &rhs) {
    direction_to_light = rhs.direction_to_light;
    light_color = rhs.light_color;
    return *this;
}

directional_light &directional_light::operator=(directional_light &&rhs) {
    direction_to_light = rhs.direction_to_light;
    light_color = rhs.light_color;
    return *this;
}

vector3f directional_light::light_vector_unit(const vector3f&) const {
    return direction_to_light;
}

color directional_light::get_color() const {
    return light_color;
}
