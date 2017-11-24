#ifndef VECTOR_UTIL_H
#define VECTOR_UTIL_H

#include "vector.h"

namespace math {
    template<typename float_t, int dim>
    vector<float_t, dim> elementwise_min(const vector<float_t, dim> &lhs, const vector<float_t, dim> &rhs);

    template<typename float_t, int dim>
    vector<float_t, dim> elementwise_max(const vector<float_t, dim> &lhs, const vector<float_t, dim> &rhs);

    template<typename float_t>
    vector<float_t, 3> around_axis(const vector<float_t, 3> &point, const vector<float_t, 3> &axis);
}

#include "vector_util.cpp.h"

#endif
