#ifndef MATERIAL_H
#define MATERIAL_H

#include "vector.h"

class texture;

class material {
public:
    material();
    ~material();

    color shade(const light_list &lights, const vector3f &surface_normal) const;
    
public:
    const math::vector3f &get_ambient_reflectivity() const;
    void set_ambient_reflectivity(const math::vector3f &vec);
    const math::vector3f &get_diffuse_reflectivity() const;
    void set_diffuse_reflectivity(const math::vector3f &vec);
    const math::vector3f &get_specular_reflectivity() const;
    void set_specular_reflectivity(const math::vector3f &vec);
    const math::vector3f &get_transmission_filter() const;
    void set_transmission_filter(const math::vector3f &vec);
    float get_dissolve() const;
    bool get_dissolve_halo() const;
    void set_dissolve(float value, bool halo);
    float get_specular_exponent() const;
    void set_specular_exponent(float value);
    float get_sharpness() const;
    void set_sharpness(float value);
    float get_optical_density() const;
    void set_optical_density(float value);
    int get_illumination_model() const;
    void set_illumination_model(int value);
    const texture *get_texture_map() const;
    void set_texture_map(const texture *t);

private:
    math::vector3f ambient_reflectivity;
    math::vector3f diffuse_reflectivity;
    math::vector3f specular_reflectivity;
    math::vector3f transmission_filter;
    float dissolve;
    float dissolve_halo;
    float specular_exponent;
    float sharpness;
    float optical_density;
    int illumination_model;

    const texture *tex;
};

#endif
