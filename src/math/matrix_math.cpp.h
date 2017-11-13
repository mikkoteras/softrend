#include "matrix.h"

namespace math {
    template<typename float_t, int rows, int columns>
    matrix<float_t, rows, columns> operator*(float_t lhs, const vector<float_t, columns> &lhs) {
        return lhs * rhs;
    }
}
