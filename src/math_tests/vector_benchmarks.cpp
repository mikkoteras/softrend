#include "vector_benchmarks.h"
#include "test_harness.h"
#include "vector.h"
#include <chrono>
#include <iostream>

using namespace math;

void vector_interpolation_benchmark();
void vector_normal_interpolation_benchmark();

void vector_benchmarks() {
    vector_interpolation_benchmark();
    vector_normal_interpolation_benchmark();
}

void vector_interpolation_benchmark() {
    vector3f left{1.0f, 2.0f, 3.0};
    vector3f right{10.0f, 11.0f, 13.0f};
    vector3f target{5.0f, 20.f, 9.0f};
    int y_steps = 100000, x_steps = 100000;

    test_harness::the_tester->start_clock();
    
    vector3f left_delta = (target - left) / y_steps;
    vector3f right_delta = (target - right) / y_steps;

    for (int y = 0; y < y_steps; ++y) {
        vector3f point = left;
        vector3f point_delta = (right - left) / x_steps;

        for (int x = 0; x < x_steps; ++x) {
            point += point_delta;

            if (point.x() > 100.0f)
                std::cout << "this won't be executed, but it keeps gcc from optimizing the entire benchmark away.";
        }

        left += left_delta;
        right += right_delta;
    }

    test_harness::the_tester->stop_clock("vector interpolation benchmark");
}

void vector_normal_interpolation_benchmark() {
    vector3f left{1.0f, 2.0f, 3.0};
    vector3f right{2.0f, 3.0f, 1.0f};
    vector3f target{3.0f, 1.f, 2.0f};

    left.normalize();
    right.normalize();
    target.normalize();
    
    int y_steps = 50000, x_steps = 50000;

    test_harness::the_tester->start_clock();
    
    vector3f left_delta = (target - left) / y_steps;
    vector3f right_delta = (target - right) / y_steps;

    for (int y = 0; y < y_steps; ++y) {
        vector3f normal = left.unit();
        vector3f normal_delta = (right.unit() - left.unit()) / x_steps;

        for (int x = 0; x < x_steps; ++x) {
            vector3f unit = normal;
            unit.normalize();
            normal += normal_delta;

            if (unit.x() > 100.0f)
                std::cout << "this won't be executed, but it keeps gcc from optimizing the entire benchmark away.";
        }

        left += left_delta;
        right += right_delta;
    }

    test_harness::the_tester->stop_clock("vector normal interpolation benchmark");
}
