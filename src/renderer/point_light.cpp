#include "point_light.h"

point_light::point_light() :
    light() {
}

point_light::point_light(const math::vector3f &position, const color &light_color) :
    light(),
    pos(position),
    col(light_color) {
}

point_light::point_light(const point_light &rhs) {
    pos = rhs.pos;
    col = rhs.col;
}

point_light::point_light(point_light &&rhs) {
    pos = rhs.pos;
    col = rhs.col;
}

const point_light &point_light::operator=(const point_light &rhs) {
    pos = rhs.pos;
    col = rhs.col;
    return *this;
}

point_light &point_light::operator=(point_light &&rhs) {
    pos = rhs.pos;
    col = rhs.col;
    return *this;
}

point_light::~point_light() {
}

const math::vector3f &point_light::position() const {
    return pos;
}

const color &point_light::get_color() const {
    return col;
}
