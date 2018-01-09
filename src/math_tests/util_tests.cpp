#include "test_harness.h"
#include "math/math_util.h"

void util_period_tests();

using namespace math;

bool is_approximately(double lhs, double rhs) {
    return abs(lhs - rhs) < 0.000001;
}

void util_tests() {
    util_period_tests();
}

void util_period_tests() {
    ASSERT(is_approximately(period<double>(0.0, 0.0, 1.0), 0.0));
    ASSERT(is_approximately(period<double>(1.0, 0.0, 1.0), 0.0));
    ASSERT(is_approximately(period<double>(0.2, 0.0, 1.0), 0.2));
    ASSERT(is_approximately(period<double>(-0.1, 0.0, 1.0), 0.9));
    ASSERT(is_approximately(period<double>(-3.2, 0.0, 1.0), 0.8));
    ASSERT(is_approximately(period<double>(1.3, 0.0, 1.0), 0.3));

    ASSERT(is_approximately(period<double>(1.4, 0.0, 3.0), 1.4));
    ASSERT(is_approximately(period<double>(-99.1, 0.0, 3.0), 2.9));
    ASSERT(is_approximately(period<double>(11.4, 0.0, 3.0), 2.4));

    ASSERT(is_approximately(period<double>(11.4, 1.0, 3.0), 1.4));
    ASSERT(is_approximately(period<double>(-6.4, 1.0, 3.0), 1.6));

    ASSERT(is_approximately(period<double>(0.5, -1.0, 2.0), 0.5));
    ASSERT(is_approximately(period<double>(-18.1, -1.0, 2.0), -0.1));
    ASSERT(is_approximately(period<double>(25.3, -1.0, 2.0), 1.3));

    ASSERT(is_approximately(period<double>(5.3, -5.0, -4.5), -4.7));
}
