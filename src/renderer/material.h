#ifndef MATERIAL_H
#define MATERIAL_H

#include "color3.h"
#include "color4.h"
#include "types.h"
#include "math/vector.h"

class light_list;
struct scene_render_context;
struct surface_position;
class texture;

class unsupported_material_exception {};

class material {
public:
    material(illumination_model_t illum);
    ~material();

    static material *create(illumination_model_t illumination_model);

    template<combined_interpolation_mode_t mode>
    color4 shade(const surface_position &point, const scene_render_context &scene) const;

    virtual color4 shade_flat(const surface_position &point, const scene_render_context &scene) const = 0;
    virtual color4 shade_gouraud(const surface_position &point, const scene_render_context &scene) const = 0;
    virtual color4 shade_phong(const surface_position &point, const scene_render_context &scene) const = 0;
    
    virtual color4 shade(const math::vector3f &surface_point, // with texture
                         const math::vector3f &surface_normal_unit,
                         const math::vector3f &point_to_eye_unit,
                         const math::vector2f &uv_coordinates,
                         const light_list &light_sources) const = 0;
    virtual color4 shade(const math::vector3f &surface_point, // without texture
                         const math::vector3f &surface_normal_unit,
                         const math::vector3f &point_to_eye_unit,
                         const light_list &light_sources) const = 0;
    color4 diffuse_texture_map(const math::vector2f &uv_coordinates) const; // diffuse map color only
    bool is_textured() const;
    bool is_colored() const;
    bool has_transparency() const;

public:
    illumination_model_t get_illumination_model() const;
    const color3 &get_ambient_reflectivity() const;
    const color3 &get_diffuse_reflectivity() const;
    const color3 &get_specular_reflectivity() const;
    const color3 &get_emissivity() const;
    float get_specular_exponent() const;
    float get_dissolve() const;
    bool get_dissolve_halo() const;
    const texture *get_ambient_map() const;
    const texture *get_diffuse_map() const;
    const texture *get_specular_map() const;
    const texture *get_emissive_map() const;
    
    void set_ambient_reflectivity(const color3 &col);
    void set_diffuse_reflectivity(const color3 &col);
    void set_specular_reflectivity(const color3 &col);
    void set_emissivity(const color3 &col);
    void set_dissolve(float value, bool halo);
    void set_specular_exponent(float value);
    void set_ambient_map(const texture *t);
    void set_diffuse_map(const texture *t);
    void set_specular_map(const texture *t);
    void set_emissive_map(const texture *t);

protected:
    color3 ambient_color_at(const math::vector2f &uv) const;
    color3 diffuse_color_at(const math::vector2f &uv) const;
    color3 specular_color_at(const math::vector2f &uv) const;
    color3 emissive_color_at(const math::vector2f &uv) const;

private:
    illumination_model_t illumination_model;
    color3 ambient_reflectivity = color3(1.0f, 1.0f, 1.0f);
    color3 diffuse_reflectivity = color3(1.0f, 1.0f, 1.0f);
    color3 specular_reflectivity = color3(1.0f, 1.0f, 1.0f);
    color3 emissivity = color3(0.0f, 0.0f, 0.0f);
    float dissolve = 1.0f;
    float dissolve_halo = false;
    float specular_exponent = 100.0f;

    const texture *ambient_map = nullptr;
    const texture *diffuse_map = nullptr;
    const texture *specular_map = nullptr;
    const texture *emissive_map = nullptr;

    bool transparency = false;
};

#include "material.cpp.h"

#endif
