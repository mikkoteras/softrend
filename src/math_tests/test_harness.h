#ifndef TEST_HARNESS_H
#define TEST_HARNESS_H

#define ASSERT(x) test_harness::the_tester->test((x), __FILE__, __LINE__)

#include <string>
#include <chrono>

class test_harness {
public:
    test_harness();
    ~test_harness();

    void test(bool cond, const std::string &filename, int line_number);
    void start_clock();
    void stop_clock(const std::string &benchmark);

    static test_harness *the_tester;

private:
    int tests_tried, tests_passed, tests_failed;
    std::chrono::high_resolution_clock::time_point benchmark_start_time;
};

#endif
