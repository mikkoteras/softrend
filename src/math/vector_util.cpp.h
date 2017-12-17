#include "math_detail.h"
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

    template<typename float_t>
    vector<float_t, 3> around_axis(const vector<float_t, 3> &point, const vector<float_t, 3> &axis, float_t angle) {
        using namespace detail;
        vector<float_t, 3> axis_unit(axis.unit());

        return cos<float_t>(angle) * point +
            sin<float_t>(angle) * (axis_unit.cross(point)) +
            (static_cast<float_t>(1) - cos<float_t>(angle)) * axis_unit.dot(point) * axis_unit;
    }

    template<typename float_t>
    vector<float_t, 3> closest_point_on_line(const vector<float_t, 3> &point,
        const vector<float_t, 3> &point_on_line, const vector<float_t, 3> &line_direction) {

        vector<float_t, 3> v(point - point_on_line);
        return point - (v - line_direction * v.dot(line_direction));
    }
}
