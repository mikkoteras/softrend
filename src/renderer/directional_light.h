#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include "light.h"
#include "color3.h"
#include "vector.h"

class directional_light : public light {
public:
    directional_light(const math::vector3f &direction, const color3 &light_color);
    directional_light(const math::vector3f &direction, const color3 &diffuse_color, const color3 &specular_color);
    ~directional_light();

    directional_light(const directional_light &rhs);
    directional_light(directional_light &&rhs);
    const directional_light &operator=(const directional_light &rhs);
    directional_light &operator=(directional_light &&rhs);

    math::vector3f light_to_surface_unit(const math::vector3f &surface_point) const override;
    math::vector3f surface_to_light_unit(const math::vector3f &surface_point) const override;
    color3 diffuse() const override;
    color3 specular() const override;

private:
    math::vector3f light_direction;
    color3 diffuse_color;
    color3 specular_color;
};

#endif
