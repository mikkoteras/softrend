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

    virtual is_fully_ambient() const = 0;

    virtual color ambient() const = 0;
    virtual color diffuse() const = 0;
    virtual color specular() const = 0;
};

#endif
