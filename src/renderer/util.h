#ifndef UTIL_H
#define UTIL_H

#include "vector.h"
#include <sstream>
#include <string>

using namespace math;

namespace util {
    template<typename float_t, int dim> std::string to_string(const vector<float_t, dim> &vec) {
        std::ostringstream result;
        result << "[";
        
        for (int i = 0; i < dim; ++i) {
            result << vec[i];

            if (i != dim - 1)
                result << " ";
        }
            
        result << "]";
        return result.str();
    }

    template<typename float_t, int dim>
    vector<float_t, dim> min (const math::vector<float_t, dim> &v1, const vector<float_t, dim> &v2) {
        vector<float_t, dim> result;

        for (int i = 0; i < dim; ++i)
            result[i] = v1[0] < v2[0] ? v1[i] : v2[i];

        return result;
    }

    template<typename float_t, int dim>
    vector<float_t, dim> max (const vector<float_t, dim> &v1, const vector<float_t, dim> &v2) {
        vector<float_t, dim> result;

        for (int i = 0; i < dim; ++i)
            result[i] = v1[0] > v2[0] ? v1[i] : v2[i];

        return result;
    }
}

#endif
