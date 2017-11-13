#include "directional_light.h"

directional_light::directional_light() :
    light() {
}

directional_light::directional_light(const math::vector3f &direction, const color &light_color) :
    light(),
    dir(direction.unit()),
    col(light_color) {
}

directional_light::directional_light(const directional_light &rhs) {
    dir = rhs.dir;
    col = rhs.col;
}

directional_light::directional_light(directional_light &&rhs) {
    dir = rhs.dir;
    col = rhs.col;
}

const directional_light &directional_light::operator=(const directional_light &rhs) {
    dir = rhs.dir;
    col = rhs.col;
    return *this;
}

directional_light &directional_light::operator=(directional_light &&rhs) {
    dir = rhs.dir;
    col = rhs.col;
    return *this;
}

directional_light::~directional_light() {
}

const math::vector3f &directional_light::direction() const {
    return dir;
}

const color &directional_light::get_color() const {
    return col;
}
