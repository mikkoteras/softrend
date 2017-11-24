#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include "math_detail.h"

namespace math {
    // Return x limited to [min, max].
    template<typename t> t clamped(t x, t min, t max) {
        if (x < min)
            return min;
        else if (x > max)
            return max;
        else
            return x;
    }

    // Limit x to [min, max].
    template<typename t> void clamp(t &x, t min, t max) {
        if (x < min)
            x = min;
        else if (x > max)
            x = max;
    }

    // Limit x to [low, high) with wraparound.
    template<typename t> t period(t x, t low, t high) {
        t length = high - low;
        t y = (x - low) / length;
        return length * (y - detail::floor(y)) + low;
    }
}

#endif
