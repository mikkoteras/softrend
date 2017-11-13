#ifndef LINEAR_TRANSFORMS_H
#define LINEAR_TRANSFORMS_H

#include "matrix.h"

namespace math { namespace linear_transforms {
    template<typename float_t> matrix<float_t, 4, 4> translate3x(float_t x);
    template<typename float_t> matrix<float_t, 4, 4> translate3y(float_t y);
    template<typename float_t> matrix<float_t, 4, 4> translate3z(float_t z);
    template<typename float_t> matrix<float_t, 4, 4> translate3(float_t x, float_t y, float_t z);

    template<typename float_t> matrix<float_t, 4, 4> rotate3x(float_t t);
    template<typename float_t> matrix<float_t, 4, 4> rotate3y(float_t t);
    template<typename float_t> matrix<float_t, 4, 4> rotate3z(float_t t);

    template<typename float_t> matrix<float_t, 4, 4> scale3x(float_t x);
    template<typename float_t> matrix<float_t, 4, 4> scale3y(float_t y);
    template<typename float_t> matrix<float_t, 4, 4> scale3z(float_t z);
    template<typename float_t> matrix<float_t, 4, 4> scale3(float_t xyz);
    template<typename float_t> matrix<float_t, 4, 4> scale3(float_t x, float_t y, float_t z);

    template<typename float_t> matrix<float_t, 3, 3> translate2x(float_t x);
    template<typename float_t> matrix<float_t, 3, 3> translate2y(float_t y);
    template<typename float_t> matrix<float_t, 3, 3> translate2(float_t x, float_t y);

    template<typename float_t> matrix<float_t, 3, 3> rotate2(float_t t);

    template<typename float_t> matrix<float_t, 3, 3> scale2x(float_t x);
    template<typename float_t> matrix<float_t, 3, 3> scale2y(float_t y);
    template<typename float_t> matrix<float_t, 3, 3> scale2(float_t xy);
    template<typename float_t> matrix<float_t, 3, 3> scale2(float_t x, float_t y);
} }

#include "linear_transforms.cpp.h"

#endif
