#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include "light.h"
#include "color.h"
#include "vector.h"

class directional_light : public light {
public:
    directional_light();
    directional_light(const math::vector3f &direction, const color &c);
    directional_light(const directional_light &rhs);
    directional_light(directional_light &&rhs);
    const directional_light &operator=(const directional_light &rhs);
    directional_light &operator=(directional_light &&rhs);
    ~directional_light();

    const math::vector3f &direction() const;
    const color &get_color() const;

private:
    math::vector3f dir;
    color col;
};

#endif
