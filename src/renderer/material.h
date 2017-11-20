#ifndef MATERIAL_H
#define MATERIAL_H

#include "vector.h"

class texture;

class material {
public:
    struct model {
        math::vector3f ambient_reflectivity;
        math::vector3f reflectivity;
        math::vector3f specular_reflectivity;
        float material_dissolve;
        float specular_exponent;
        int illumination_model;
    };
    
public:
    material() = delete;
    material(const model &data, const texture *t);
    ~material();

    const texture *get_texture() const;

private:
    model data;
    const texture *tex;
};

#endif
