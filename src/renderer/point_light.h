#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "light.h"
#include "color.h"
#include "vector.h"

class point_light : public light {
public:
    point_light(const math::vector3f &position, const color &light_color);
    ~point_light();

    point_light(const point_light &rhs);
    point_light(point_light &&rhs);
    const point_light &operator=(const point_light &rhs);
    point_light &operator=(point_light &&rhs);

    math::vector3f light_vector_unit(const math::vector3f &surface_point) const override;
    color get_color() const override;

private:
    math::vector3f position;
    color light_color;
};

#endif
