#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"

class color;

class light {
public:
    light();
    virtual ~light();

    light(const light&) = delete;
    light(light&&) = delete;
    const light &operator=(const light&) = delete;
    light &operator=(light&&) = delete;

    virtual void sum(const math::vector3f &surface_normal, color &shade) const = 0;
};

#endif
