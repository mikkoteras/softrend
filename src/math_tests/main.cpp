#include "linear_transform_tests.h"
#include "matrix_tests.h"
#include "test_harness.h"
#include "vector_tests.h"
#include <iostream>

void run_all_tests();

int main()
{
    run_all_tests();

    //char dummy;
    //std::cout << "Ready to exit..." << std::endl;
    //std::cin >> dummy;
    return 0;
}

void run_all_tests() {
    test_harness::the_tester = new test_harness;
    vector_tests();
    matrix_tests();
    linear_transform_tests();
    delete test_harness::the_tester;
}
