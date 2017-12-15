#ifndef LIGHT_H
#define LIGHT_H

#include "color.h"
#include "vector.h"

class light {
public:
    light();
    virtual ~light();

    light(const light&);
    light(light&&);
    const light &operator=(const light&);
    light &operator=(light&&);

    virtual math::vector3f light_to_surface_unit(const math::vector3f &surface_point) const = 0;
    virtual math::vector3f surface_to_light_unit(const math::vector3f &surface_point) const = 0;
    virtual color diffuse() const = 0;
    virtual color specular() const = 0;
};

#endif
