#include <algorithm>

namespace math {
    template<typename float_t, int dim>
    vector<float_t, dim> elementwise_min(const vector<float_t, dim> &lhs, const vector<float_t, dim> &rhs) {
        vector<float_t, dim> result;

        for (int i = 0; i < dim; ++i)
            result[i] = std::min(lhs[i], rhs[i]);

        return result;
    }

    template<typename float_t, int dim>
    vector<float_t, dim> elementwise_max(const vector<float_t, dim> &lhs, const vector<float_t, dim> &rhs) {
        vector<float_t, dim> result;

        for (int i = 0; i < dim; ++i)
            result[i] = std::max(lhs[i], rhs[i]);

        return result;
    }
}
