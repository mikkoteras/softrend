#ifndef SOFTREND_TYPES_H
#define SOFTREND_TYPES_H

enum reflection_model_t {
    lambert,
    gouraud,
    phong,
    blinn_phong,
    max_reflection_model = blinn_phong
};

#endif
