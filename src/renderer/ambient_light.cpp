#include "ambient_light.h"
#include "color.h"

ambient_light::ambient_light(const color &light_color) :
    light(),
    col(light_color) {
}

ambient_light::~ambient_light() {
}

bool ambient_light::is_fully_ambient() override const {
    return true;
}

color ambient_light::ambient() const {
    return col;
}

color ambient_light::diffuse() const {
    return color(0, 0, 0, 1);
}

color ambient_light::specular() const {
    return color(0, 0, 0, 1);
}
