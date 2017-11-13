#ifndef VIEWPORT_TRANSFORMS_H
#define VIEWPORT_TRANSFORMS_H

#include "matrix.h"
#include "vector.h"

namespace math { namespace viewport_transforms {
    template<typename float_t>
    matrix<float_t, 4, 4> world_to_view_plane(const vector<float_t, 3> &eye_position,
                                              const vector<float_t, 3> &eye_direction,
                                              const vector<float_t, 3> &up_vector);
    template<typename float_t>
    matrix<float_t, 4, 4> simple_perspective_projection(float_t reference_to_view_plane_distance);
} }

#include "viewport_transforms.cpp.h"

#endif
