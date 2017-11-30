#ifndef UTIL_H
#define UTIL_H

#include "types.h"
#include "vector.h"
#include <sstream>
#include <string>

namespace util {
    template<typename float_t, int dim> std::string to_string(const math::vector<float_t, dim> &vec) {
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

    std::string to_string(reflection_model_t model);
}

#endif
