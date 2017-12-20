#include "vector_benchmarks.h"
#include "test_harness.h"
#include "vector.h"
#include <chrono>
#include <iostream>

using namespace math;

void vector_interpolation_test();

void vector_benchmarks() {
    vector_interpolation_test();
}

void vector_interpolation_test() {
    test_harness::the_tester->start_clock();
    
    vector3f left{1.0f, 2.0f, 3.0};
    vector3f right{10.0f, 11.0f, 13.0f};
    vector3f target{5.0f, 20.f, 9.0f};
    int y_steps = 100000, x_steps = 100000;

    vector3f left_delta = (target - left) / y_steps;
    vector3f right_delta = (target - right) / y_steps;

    for (int y = 0; y < y_steps; ++y) {
        vector3f point = left;
        vector3f point_delta = (right - left) / x_steps;

        for (int x = 0; x < x_steps; ++x)
            // supposedly do something with the point here
            point += point_delta;

        left += left_delta;
        right += right_delta;
    }

    test_harness::the_tester->stop_clock("vector interpolation test");
}
