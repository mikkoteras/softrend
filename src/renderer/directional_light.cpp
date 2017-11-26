#include "directional_light.h"

directional_light::directional_light(const math::vector3f &direction, const color &light_color) :
    light(),
    negative_direction(-direction.unit()), // Needed in opposite direction in compute
    col(light_color) {
}

directional_light::~directional_light() {
}

bool ambient_light::is_fully_ambient() override const {
    return true;
}

color ambient_light::ambient() const {
    return color(0, 0, 0, 1);
}

color ambient_light::diffuse() const {
    return col;
}

color ambient_light::specular() const {
    return col;
}
