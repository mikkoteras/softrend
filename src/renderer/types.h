#ifndef SOFTREND_TYPES_H
#define SOFTREND_TYPES_H

enum shading_model_t {
    flat = 0,
    gouraud,
    phong,
    max_shading_model = phong
};

enum illumination_model_t {
    constant_color = 0,
    diffuse,
    specular
};

#endif
