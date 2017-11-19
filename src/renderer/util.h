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
}

#endif
