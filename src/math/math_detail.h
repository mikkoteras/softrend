#ifndef MATH_DETAIL_H
#define MATH_DETAIL_H

#include <math.h>

namespace math { namespace detail {
    template<typename float_t> constexpr float_t sin(float_t t);
    template<typename float_t> constexpr float_t cos(float_t t);
    template<typename float_t> constexpr float_t asin(float_t t);
    template<typename float_t> constexpr float_t acos(float_t t);
    template<typename float_t> constexpr float_t floor(float_t t);
    
    template<typename float_t> constexpr float_t pi();

    template<> constexpr float sin<float>(float t)   { return ::sinf(t); }
    template<> constexpr float cos<float>(float t)   { return ::cosf(t); }
    template<> constexpr float asin<float>(float t)  { return ::asinf(t); }
    template<> constexpr float acos<float>(float t)  { return ::acosf(t); }
    template<> constexpr float floor<float>(float t) { return ::floorf(t); }

    template<> constexpr double sin<double>(double t)   { return ::sin(t); }
    template<> constexpr double cos<double>(double t)   { return ::cos(t); }
    template<> constexpr double asin<double>(double t)  { return ::asin(t); }
    template<> constexpr double floor<double>(double t) { return ::floor(t); }

    template<> constexpr float pi() { return 3.14159265358979323846f; }
    template<> constexpr double pi() { return 3.14159265358979323846; }
} }

#endif
