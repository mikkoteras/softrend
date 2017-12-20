#include "test_harness.h"
#include <iostream>

test_harness *test_harness::the_tester = nullptr;

test_harness::test_harness() :
    tests_tried(0),
    tests_passed(0),
    tests_failed(0) {
}

test_harness::~test_harness() {
    if (tests_failed)
        std::cout << "------------" << std::endl;

    std::cout << "Tests: " << tests_tried << std::endl
              << "  passed: " << tests_passed << std::endl
              << "  failed: " << tests_failed << std::endl;
}

void test_harness::test(bool cond, const std::string &filename, int line_number) {
    ++tests_tried;

    if (cond) {
        ++tests_passed;
    }
    else {
        std::cout << "Test failed in " << filename << ", line " << line_number << std::endl;
        ++tests_failed;
    }
}
