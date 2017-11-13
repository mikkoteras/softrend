#ifndef MATRIX_MATH_H
#define MATRIX_MATH_H

namespace math {
    template<typename float_t, int rows, int columns> class matrix;
    template<typename float_t, int rows, int columns> operator*(float_t lhs, const vector<float_t, columns> &lhs);
}

#include "matrix_math.cpp.h"

#endif
