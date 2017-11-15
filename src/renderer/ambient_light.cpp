#include "ambient_light.h"
#include "color.h"

ambient_light::ambient_light(const color &light_color) :
    light(),
    col(light_color) {
}

ambient_light::~ambient_light() {
}

void ambient_light::sum(const math::vector3f&, color &shade) const {
    shade += col;
}
