#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include "light.h"
#include "color.h"
#include "vector.h"

class directional_light : public light {
public:
    directional_light(const math::vector3f &direction, const color &light_color);
    directional_light(const math::vector3f &direction, const color &diffuse_color, const color &specular_color);
    ~directional_light();

    directional_light(const directional_light &rhs);
    directional_light(directional_light &&rhs);
    const directional_light &operator=(const directional_light &rhs);
    directional_light &operator=(directional_light &&rhs);

    math::vector3f light_to_surface_unit(const math::vector3f &surface_point) const override;
    math::vector3f surface_to_light_unit(const math::vector3f &surface_point) const override;
    color diffuse() const override;
    color specular() const override;

private:
    math::vector3f light_direction;
    color diffuse_color;
    color specular_color;
};

#endif
