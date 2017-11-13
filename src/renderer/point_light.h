#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "light.h"
#include "color.h"
#include "vector.h"

class point_light : public light {
public:
    point_light();
    point_light(const math::vector3f &position, const color &c);
    point_light(const point_light &rhs);
    point_light(point_light &&rhs);
    const point_light &operator=(const point_light &rhs);
    point_light &operator=(point_light &&rhs);
    ~point_light();

    const math::vector3f &position() const;
    const color &get_color() const;

private:
    math::vector3f pos;
    color col;
};

#endif
