#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "vector.h"

class light_list;
class texture;

class material {
public:
    material();
    ~material();

    color shade_flat(const math::vector3f &surface_midpoint, const math::vector3f &surface_normal_unit,
                     const light_list &light_sources) const;
    color shade_phong(const math::vector3f &surface_point, const math::vector3f &surface_normal_unit,
                      const math::vector3f &point_to_eye_unit, const light_list &light_sources) const;

public:
    const color &get_ambient_reflectivity() const;
    const color &get_diffuse_reflectivity() const;
    const color &get_specular_reflectivity() const;
    const color &get_transmission_filter() const;
    bool get_dissolve_halo() const;
    float get_specular_exponent() const;
    float get_dissolve() const;
    float get_sharpness() const;
    float get_optical_density() const;
    int get_illumination_model() const;
    const texture *get_texture_map() const;
    
    void set_ambient_reflectivity(const color &col);
    void set_diffuse_reflectivity(const color &col);
    void set_specular_reflectivity(const color &col);
    void set_transmission_filter(const color &col);
    void set_dissolve(float value, bool halo);
    void set_specular_exponent(float value);
    void set_sharpness(float value);
    void set_optical_density(float value);
    void set_illumination_model(int value);
    void set_texture_map(const texture *t);

private:
    color ambient_reflectivity;
    color diffuse_reflectivity;
    color specular_reflectivity;
    color transmission_filter;
    float dissolve;
    float dissolve_halo;
    float specular_exponent;
    float sharpness;
    float optical_density;
    int illumination_model;

    const texture *tex;
};

#endif
