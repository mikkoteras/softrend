#include "light_list.h"
#include "directional_light.h"
#include "light.h"
#include "point_light.h"

using namespace std;

light_list::light_list() :
    ambient_light(0.0f, 0.0f, 0.0f) {
}

light_list::~light_list() {
    for (auto l: per_pixel_lights)
        delete l;

    per_pixel_lights.clear();
}

void light_list::clear() {
    ambient_light = color3(0.0f, 0.0f, 0.0f);
    per_pixel_lights.clear();
}

void light_list::set_ambient_light(const color3 &col) {
    ambient_light = col;
}

void light_list::add_light(const directional_light &l) {
    per_pixel_lights.push_back(new directional_light(l));
}

void light_list::add_light(const point_light &l) {
    per_pixel_lights.push_back(new point_light(l));
}

color3 light_list::ambient_coeff() const {
    return ambient_light;
}

const std::vector<light*> &light_list::get() const {
    return per_pixel_lights;
}
