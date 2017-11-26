#ifndef AMBIENT_LIGHT_H
#define AMBIENT_LIGHT_H

#include "light.h"
#include "color.h"

class ambient_light : public light {
public:
    ambient_light(const color &light_color);
    ~ambient_light();

    ambient_light(const ambient_light&) = delete;
    ambient_light(ambient_light&&) = delete;
    const ambient_light &operator=(const ambient_light&) = delete;
    ambient_light &operator=(ambient_light&&) = delete;

    bool is_fully_ambient() override const;

    color ambient() override const;
    color diffuse() override const;
    color specular() override const;

private:
    color col;
};

#endif
