#include <math.h>
#include "math_detail.h"

namespace math { namespace viewport_transforms {
    template<typename float_t>
    matrix<float_t, 4, 4> world_to_view_plane(const vector<float_t, 3> &eye_position,
                                              const vector<float_t, 3> &eye_direction,
                                              const vector<float_t, 3> &up_vector) {
        matrix<float_t, 4, 4> translate{
            1, 0, 0, -eye_position[0],
            0, 1, 0, -eye_position[1],
            0, 0, 1, -eye_position[2],
            0, 0, 0, 1
        };

        vector<float_t, 3> n = eye_direction.unit();
        vector<float_t, 3> u = up_vector.cross(eye_direction).unit();
        vector<float_t, 3> v = n.cross(u);
        
        matrix<float_t, 4, 4> rotate{
            u[0], u[1], u[2], 0,
            v[0], v[1], v[2], 0,
            n[0], n[1], n[2], 0,
            0,    0,    0,    1
        };

        return rotate * translate;
    }

    template<typename float_t>
        matrix<float_t, 4, 4> simple_perspective_projection(float_t reference_to_view_plane_distance) {
        // Adapted from Hearn, page 445 (z vector is just unity as is breaks depth buffering otherwise)
        const float_t dp = reference_to_view_plane_distance;
        return matrix<float_t, 4, 4> {
            1, 0, 0,         0,
            0, 1, 0,         0,
            0, 0, 1,         0,
            0, 0, -1 / dp,   1
        };
    }
} }
