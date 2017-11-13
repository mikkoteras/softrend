#ifndef MATRIX_TYPEDEFS_H
#define MATRIX_TYPEDEFS_H

#include "matrix.h"

namespace math {
    typedef matrix<float, 2, 2> matrix2x2f;
    typedef matrix<float, 3, 3> matrix3x3f;
    typedef matrix<float, 4, 4> matrix4x4f;
    typedef matrix<double, 2, 2> matrix2x2d;
    typedef matrix<double, 3, 3> matrix3x3d;
    typedef matrix<double, 4, 4> matrix4x4d;
}

#endif
