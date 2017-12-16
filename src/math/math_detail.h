#ifndef MATH_DETAIL_H
#define MATH_DETAIL_H

namespace math {
    namespace detail {
        template<typename float_t> float_t sin(float_t t);
        template<typename float_t> float_t cos(float_t t);
        template<typename float_t> float_t asin(float_t t);
        template<typename float_t> float_t acos(float_t t);
        template<typename float_t> float_t floor(float_t t);

        template<typename float_t> constexpr float_t pi();
    }
}

#endif
