#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

#include "vector.h"

namespace math {
    template<typename float_t> vector<float_t, 2> normalize_projected(const vector<float_t, 4> &v);
}

#include "vector_math.cpp.h"

#endif
