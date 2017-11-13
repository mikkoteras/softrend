#ifndef MATH_UTIL_H
#define MATH_UTIL_H

namespace math {
    template<typename t> t clamp(t x, t min, t max) {
        if (x < min)
            return min;
        else if (x > max)
            return max;
        else
            return x;
    }
}

#endif
