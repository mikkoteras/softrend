#include "math_detail.h"
#include <cmath>

namespace math {
    namespace detail {
        template<> float sin<float>(float t) { return std::sinf(t); }
        template<> float cos<float>(float t) { return std::cosf(t); }
        template<> float asin<float>(float t) { return std::asinf(t); }
        template<> float acos<float>(float t) { return std::acosf(t); }
        template<> float floor<float>(float t) { return std::floorf(t); }

        template<> double sin<double>(double t) { return std::sin(t); }
        template<> double cos<double>(double t) { return std::cos(t); }
        template<> double asin<double>(double t) { return std::asin(t); }
        template<> double acos<double>(double t) { return std::acos(t); }
        template<> double floor<double>(double t) { return std::floor(t); }

        template<> float pi<float>() { return 3.14159265358979323846f; }
        template<> double pi<double>() { return 3.14159265358979323846; }
    }
}
