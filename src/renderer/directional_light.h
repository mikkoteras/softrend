#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include "light.h"
#include "color.h"
#include "vector.h"

class directional_light : public light {
public:
    directional_light(const math::vector3f &direction, const color &light_color);
    ~directional_light();

    directional_light(const directional_light &rhs);
    directional_light(directional_light &&rhs);
    const directional_light &operator=(const directional_light &rhs);
    directional_light &operator=(directional_light &&rhs);

    const math::vector3f &light_vector_unit() const override;
    color get_color() const override;

private:
    math::vector3f direction_to_light;
    color light_color;
};

#endif
