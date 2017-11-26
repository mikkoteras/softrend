#include "light_list.h"
#include "light.h"

using namespace std;

light_list::light_list() {
}

light_list::~light_list() {
}

void light_list::add(unique_ptr<light> l) {
    if (l->has_diffuse_component)
        diffuse_lights.push_back(move(l));
    else
        specular_lights.push_back(move(l));
}

color light_list::sum(const math::vector3f &surface_normal) const {
    color shade(0.0f, 0.0f, 0.0f, 1.0f);

    for (const auto &l: lights)
        l->sum(surface_normal, shade);

    shade.clamp();
    return shade;
}
