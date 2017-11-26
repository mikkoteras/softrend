#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include "light.h"
#include "color.h"
#include "vector.h"

class directional_light : public light {
public:
    directional_light(const math::vector3f &direction, const color &light_color);
    ~directional_light();

    directional_light(const directional_light&) = delete;
    directional_light(directional_light&&) = delete;
    const directional_light &operator=(const directional_light&) = delete;
    directional_light &operator=(directional_light&&) = delete;

    bool is_fully_ambient() override const;

    color ambient() override const;
    color diffuse() override const;
    color specular() override const;

private:
    math::vector3f negative_direction;
    color col;
};

#endif
