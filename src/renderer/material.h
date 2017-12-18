#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "types.h"
#include "vector.h"

class light_list;
class texture;

class unsupported_material_exception {};

class material {
public:
    material(illumination_model_t illum);
    ~material();

    static material *create(int illumination_model);
    
    virtual color shade_flat(const math::vector3f &surface_midpoint, const math::vector3f &surface_normal_unit,
                             const light_list &light_sources) const = 0;
    virtual color shade_phong(const math::vector3f &surface_point, const math::vector3f &surface_normal_unit,
                              const math::vector3f &point_to_eye_unit, const light_list &light_sources) const = 0;


public:
    illumination_model_t get_illumination_model() const;
    const color &get_ambient_reflectivity() const;
    const color &get_diffuse_reflectivity() const;
    const color &get_specular_reflectivity() const;
    const color &get_emissivity() const;
    const color &get_transmission_filter() const;
    float get_specular_exponent() const;
    float get_dissolve() const;
    bool get_dissolve_halo() const;
    float get_sharpness() const;
    float get_optical_density() const;
    const texture *get_texture_map() const;
    
    void set_ambient_reflectivity(const color &col);
    void set_diffuse_reflectivity(const color &col);
    void set_specular_reflectivity(const color &col);
    void set_emissivity(const color &col);
    void set_transmission_filter(const color &col);
    void set_dissolve(float value, bool halo);
    void set_specular_exponent(float value);
    void set_sharpness(float value);
    void set_optical_density(float value);
    void set_texture_map(const texture *t);

private:
    illumination_model_t illumination_model;
    color ambient_reflectivity = color(1.0f, 1.0f, 1.0f, 1.0f);
    color diffuse_reflectivity = color(1.0f, 1.0f, 1.0f, 1.0f);
    color specular_reflectivity = color(1.0f, 1.0f, 1.0f, 1.0f);
    color emissivity = color(0.0f, 0.0f, 0.0f, 1.0f);
    color transmission_filter = color(1.0f, 1.0f, 1.0f, 1.0f);
    float dissolve = 0.0f;
    float dissolve_halo = false;
    float specular_exponent = 100.0f;
    float sharpness = 60.0f;
    float optical_density = 1.0f;

    const texture *tex = nullptr;
};

#endif
