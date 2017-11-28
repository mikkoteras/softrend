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

    virtual const math::vector3f &light_vector_unit() const = 0;
    virtual color get_color() const = 0;
};

#endif
