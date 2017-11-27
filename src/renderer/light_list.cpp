#include "light_list.h"
#include "directional_light.h"
#include "light.h"

using namespace std;

light_list::light_list() :
    ambient_light(color(0.0f, 0.0f, 0.0f, 1.0f)) {
}

light_list::~light_list() {
    for (auto l: per_pixel_lights)
        delete l;

    per_pixel_lights.clear();
}

void light_list::set_ambient_light(const color &col) {
    ambient_light = col;
}

void light_list::add_directional_light(const directional_light &l) {
    per_pixel_lights.push_back(new directional_light(l));
}

color light_list::ambient_coeff() const {
    return ambient_light;
}

const std::vector<light*> &light_list::get() const {
    return per_pixel_lights;
}
