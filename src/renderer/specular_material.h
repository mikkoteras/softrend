#ifndef SPECULAR_MATERIAL_H
#define SPECULAR_MATERIAL_H

#include "material.h"
#include "color4.h"

struct scene_render_context;
struct surface_position;

// material.mtl: illum = 2
class specular_material : public material {
public:
    specular_material();
    ~specular_material();

    color4 shade_pixel(const surface_position &point, const scene_render_context &scene) const override;
    color3 shade_vertex(const surface_position &point, const scene_render_context &scene) const override;
    color4 interpolate(const surface_position &point, const scene_render_context &scene) const override;
};

#endif
