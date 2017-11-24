#include "test_harness.h"
#include "matrix_tests.h"
#include "matrix.h"

void matrix_creation_tests();
void matrix_comparison_tests();
void matrix_artithmetic_tests();

using namespace math;

void matrix_tests() {
    matrix_creation_tests();
    matrix_comparison_tests();
    matrix_artithmetic_tests();
}

void matrix_creation_tests() {
    matrix3x3f m0(matrix3x3f::zero());
    ASSERT(m0.at(1, 1) == 0);

    matrix3x3d m1 = matrix3x3d::identity();
    ASSERT(m1.at(0, 0) == 1);
    ASSERT(m1.at(1, 0) == 0);
    ASSERT(m1.at(0, 1) == 0);
    ASSERT(m1.at(1, 1) == 1);

    matrix3x3f m2{1, 2, 3, 4, 5, 6, 7, 8, 9};
    ASSERT(m2.at(0, 0) == 1);
    ASSERT(m2.at(1, 0) == 2);
    ASSERT(m2.at(0, 1) == 4);
    ASSERT(m2.at(2, 2) == 9);

    m2.at(0, 0) = 2;
    ASSERT(m2.at(0, 0) == 2);

    matrix3x3f m3;
    m3 = m2;
    ASSERT(m3.at(2, 2) == 9);
}

void matrix_comparison_tests() {
    matrix3x3f z = matrix3x3f::zero();
    matrix3x3f o = matrix3x3f::identity();
    ASSERT(o != z);
    z.at(0, 0) = z.at(1, 1) = z.at(2, 2) = 1;
    ASSERT(o == z);
}

void matrix_artithmetic_tests() {
    matrix3x3f a{3, 4, 5,  6,  7,  8,  9, 10, 11};
    matrix3x3f b{1, 2, 3,  4,  5,  6,  7,  8,  9};
    matrix3x3f c{4, 6, 8, 10, 12, 14, 16, 18, 20};
    ASSERT(a + b == c);
    ASSERT(c - b == a);

    matrix<float, 2, 3> m{0, 4, -2, -4, -3, 0};
    matrix<float, 3, 2> n{0, 1, 1, -1, 2, 3};
    matrix<float, 2, 2> o = m * n;
    ASSERT((o == matrix<float, 2, 2>{0, -10, -3, -1}));

    matrix4x4f translate_xyz{
        1, 0, 0, 2,
        0, 1, 0, 4,
        0, 0, 1, 6,
        0, 0, 0, 1
    };
    vector4f original_point{1, 2, 3, 1};
    vector4f translated_point{3, 6, 9, 1};
    ASSERT((translate_xyz * original_point == translated_point));
}
