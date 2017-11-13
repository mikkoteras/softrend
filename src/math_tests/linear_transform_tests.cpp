#include "linear_transform_tests.h"
#include "linear_transforms.h"
#include "matrix.h"
#include "test_harness.h"
#include "vector.h"

void translation_tests();
void scale_tests();
void rotation_tests();
void combined_tests();

using namespace math;
using namespace math::linear_transforms;

template<typename float_t, int dim> bool is_approximately(const vector<float_t, dim> &lhs, const vector<float_t, dim> &rhs) {
    for (int i = 0; i < dim; ++i) {
        float_t diff = lhs[i] - rhs[i];
        
        if (diff < -0.00001 || diff > 0.00001)
            return false;
    }

    return true;
}

void linear_transform_tests() {
    translation_tests();
    scale_tests();
    rotation_tests();
    combined_tests();
}

void translation_tests() {
    vector4f a{5, 5, 5, 1};
    ASSERT((translate3x<float>(3) * a == vector4f{8, 5, 5, 1}));
    ASSERT((translate3y<float>(3) * a == vector4f{5, 8, 5, 1}));
    ASSERT((translate3z<float>(3) * a == vector4f{5, 5, 8, 1}));
    ASSERT((translate3<float>(1, 2, 4) * a == vector4f{6, 7, 9, 1}));

    vector3d b{3, -2, 1};
    ASSERT((translate2x<double>(-3) * b == vector3d{0, -2, 1}));
    ASSERT((translate2y<double>(-3) * b == vector3d{3, -5, 1}));
    ASSERT((translate2<double>(1, -4) * b == vector3d{4, -6, 1}));
}

void scale_tests() {
    vector4f a{-1, 1, 4, 1};
    ASSERT((scale3x<float>(3) * a == vector4f{-3, 1, 4, 1}));
    ASSERT((scale3y<float>(3) * a == vector4f{-1, 3, 4, 1}));
    ASSERT((scale3z<float>(3) * a == vector4f{-1, 1, 12, 1}));
    ASSERT((scale3<float>(4) * a == vector4f{-4, 4, 16, 1}));
    ASSERT((scale3<float>(2, -2, 3) * a == vector4f{-2, -2, 12, 1}));

    vector3d b{-3, 2, 1};
    ASSERT((scale2x<double>(7) * b == vector3d{-21, 2, 1}));
    ASSERT((scale2y<double>(-5) * b == vector3d{-3, -10, 1}));
    ASSERT((scale2<double>(4) * b == vector3d{-12, 8, 1}));   
    ASSERT((scale2<double>(2, -2) * b == vector3d{-6, -4, 1}));   
}

void rotation_tests() {
    const float pi = M_PI;
    vector4f a{10, 0, 0, 1};
    vector4f b = rotate3z<float>(pi / 2) * a;
    vector4f c = rotate3x<float>(pi / 2) * b;
    vector4f d = rotate3y<float>(pi / 2) * c;
    ASSERT(is_approximately(b, vector4f{0, -10, 0, 1}));
    ASSERT(is_approximately(c, vector4f{0, 0, 10, 1}));
    ASSERT(is_approximately(d, vector4f{-10, 0, 0, 1}));
}

void combined_tests() {
}
