#ifndef UTIL_H
#define UTIL_H

#include "bounding_box.h"
#include "types.h"
#include "math/matrix.h"
#include "math/vector.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

namespace util {
    template<typename float_t, int dim>
    std::string to_string(const math::vector<float_t, dim> &vec) {
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

    template<typename float_t, int rows, int columns>
    std::string to_string(const math::matrix<float_t, rows, columns> &mat, bool with_newlines = true) {
        size_t cell_width = 0;
        
        for (int y = 0; y < rows; ++y)
            for (int x = 0; x < columns; ++x) {
                std::ostringstream measure;
                measure << mat.at(x, y);
                cell_width = std::max(cell_width, measure.str().length());
            }
        
        std::ostringstream result;

        for (int y = 0; y < rows; ++y) {
            result << "[ ";

            for (int x = 0; x < columns; ++x)
                result << std::setw(cell_width) << mat.at(x, y) << " ";

            result << "]";

            if (with_newlines)
                result << std::endl;
            else
                result << " ";
        }

        return result.str();
    }

    std::string to_string(shading_model_t model);
    std::string to_string(const bounding_box &box);
}

#endif
