#include <math.h>
#include "math_detail.h"

namespace math { namespace linear_transforms {
    template<typename float_t> matrix<float_t, 4, 4> translate3x(float_t x) {
        return matrix<float_t, 4, 4>{
            1, 0, 0, x,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 4, 4> translate3y(float_t y) {
        return matrix<float_t, 4, 4>{
            1, 0, 0, 0,
            0, 1, 0, y,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 4, 4> translate3z(float_t z) {
        return matrix<float_t, 4, 4>{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, z,
            0, 0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 4, 4> translate3(float_t x, float_t y, float_t z) {
        return matrix<float_t, 4, 4>{
            1, 0, 0, x,
            0, 1, 0, y,
            0, 0, 1, z,
            0, 0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 4, 4> rotate3x(float_t t) {
        float_t s = detail::sin<float_t>(t);
        float_t c = detail::cos<float_t>(t);

        return matrix<float_t, 4, 4>{
            1,  0, 0, 0,
            0,  c, s, 0,
            0, -s, c, 0,
            0,  0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 4, 4> rotate3y(float_t t) {
        float_t s = detail::sin<float_t>(t);
        float_t c = detail::cos<float_t>(t);

        return matrix<float_t, 4, 4>{
            c, 0, -s, 0,
            0, 1,  0, 0,
            s, 0,  c, 0,
            0, 0,  0, 1
        };
    }

    template<typename float_t> matrix<float_t, 4, 4> rotate3z(float_t t) {
        float_t s = detail::sin<float_t>(t);
        float_t c = detail::cos<float_t>(t);

        return matrix<float_t, 4, 4>{
             c, s, 0, 0,
            -s, c, 0, 0,
             0, 0, 1, 0,
             0, 0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 4, 4> scale3x(float_t x) {
        return matrix<float_t, 4, 4>{
            x, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 4, 4> scale3y(float_t y) {
        return matrix<float_t, 4, 4>{
            1, 0, 0, 0,
            0, y, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 4, 4> scale3z(float_t z) {
        return matrix<float_t, 4, 4>{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 4, 4> scale3(float_t xyz) {
        return matrix<float_t, 4, 4>{
            xyz, 0,   0,   0,
            0,   xyz, 0,   0,
            0,   0,   xyz, 0,
            0,   0,   0,   1
        };
    }

    template<typename float_t> matrix<float_t, 4, 4> scale3(float_t x, float_t y, float_t z) {
        return matrix<float_t, 4, 4>{
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 3, 3> translate2x(float_t x) {
        return matrix<float_t, 3, 3>{
            1, 0, x,
            0, 1, 0,
            0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 3, 3> translate2y(float_t y) {
        return matrix<float_t, 3, 3>{
            1, 0, 0,
            0, 1, y,
            0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 3, 3> translate2(float_t x, float_t y) {
        return matrix<float_t, 3, 3>{
            1, 0, x,
            0, 1, y,
            0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 3, 3> rotate2(float_t t) {
        float_t s = detail::sin<float_t>(t);
        float_t c = detail::cos<float_t>(t);

        return matrix<float_t, 3, 3>{
             c, s, 0,
            -s, c, 0,
             0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 3, 3> scale2x(float_t x){
        return matrix<float_t, 3, 3>{
            x, 0, 0,
            0, 1, 0,
            0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 3, 3> scale2y(float_t y) {
        return matrix<float_t, 3, 3>{
            1, 0, 0,
            0, y, 0,
            0, 0, 1
        };
    }

    template<typename float_t> matrix<float_t, 3, 3> scale2(float_t xy) {
        return matrix<float_t, 3, 3>{
            xy, 0,  0,
            0,  xy, 0,
            0,  0,  1
        };
    }

    template<typename float_t> matrix<float_t, 3, 3> scale2(float_t x, float_t y) {
        return matrix<float_t, 3, 3>{
            x, 0, 0,
            0, y, 0,
            0, 0, 1
        };
    }
} }
