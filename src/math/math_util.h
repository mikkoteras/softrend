#ifndef MATH_UTIL_H
#define MATH_UTIL_H

namespace math {
    template<typename t> t clamped(t x, t min, t max) {
        if (x < min)
            return min;
        else if (x > max)
            return max;
        else
            return x;
    }

    template<typename t> void clamp(t &x, t min, t max) {
        if (x < min)
            x = min;
        else if (x > max)
            x = max;
    }
}

#endif
