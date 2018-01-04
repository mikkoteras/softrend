#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "light.h"
#include "color3.h"
#include "vector.h"

class point_light : public light {
public:
    point_light(const math::vector3f &position, const color3 &light_color3);
    point_light(const math::vector3f &position, const color3 &diffuse_color3, const color3 &specular_color3);
    ~point_light();

    point_light(const point_light &rhs);
    point_light(point_light &&rhs);
    const point_light &operator=(const point_light &rhs);
    point_light &operator=(point_light &&rhs);

    math::vector3f light_to_surface_unit(const math::vector3f &surface_point) const override;
    math::vector3f surface_to_light_unit(const math::vector3f &surface_point) const override;
    color3 diffuse() const override;
    color3 specular() const override;

private:
    math::vector3f position;
    color3 diffuse_color;
    color3 specular_color;
};

#endif
