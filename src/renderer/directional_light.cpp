#include "directional_light.h"

directional_light::directional_light(const math::vector3f &direction, const color &light_color) :
    light(),
    negative_direction(-direction.unit()), // Needed in opposite direction in compute
    col(light_color) {
}

directional_light::~directional_light() {
}

void directional_light::sum(const math::vector3f &surface_normal, color &shade) const {
    float cos_incident_angle = surface_normal.dot(negative_direction);

    if (cos_incident_angle < 0.0f)
        cos_incident_angle = 0.0f;
    
    shade += cos_incident_angle * col;
}
