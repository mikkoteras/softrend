#include "linear_transform_tests.h"
#include "matrix_tests.h"
#include "test_harness.h"
#include "util_tests.h"
#include "vector_tests.h"
#include "vector_benchmarks.h"
#include <iostream>

void run_everything();

int main()
{
    run_everything();

    return 0;
}

void run_everything() {
    test_harness harness;
    
    vector_tests();
    matrix_tests();
    linear_transform_tests();
    util_tests();
    vector_benchmarks();
}
