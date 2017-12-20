#include "test_harness.h"
#include <iostream>

using namespace std;
using namespace std::chrono;

test_harness *test_harness::the_tester = nullptr;

test_harness::test_harness() :
    tests_tried(0),
    tests_passed(0),
    tests_failed(0) {

    the_tester = this;
}

test_harness::~test_harness() {
    if (tests_failed)
        cout << "------------" << endl;

    cout << "Tests: " << tests_tried << endl
         << "  passed: " << tests_passed << endl
         << "  failed: " << tests_failed << endl;

    the_tester = nullptr;
}

void test_harness::test(bool cond, const string &filename, int line_number) {
    ++tests_tried;

    if (cond) {
        ++tests_passed;
    }
    else {
        cout << "Test failed in " << filename << ", line " << line_number << endl;
        ++tests_failed;
    }
}

void test_harness::start_clock() {
    benchmark_start_time = high_resolution_clock::now();
}

void test_harness::stop_clock(const string &benchmark) {
    high_resolution_clock::time_point benchmark_end_time = high_resolution_clock::now();
    float dur = duration<float, milli>(benchmark_end_time - benchmark_start_time).count();
    cout << benchmark << ": " << dur << " ms" << endl;
}
