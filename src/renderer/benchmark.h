#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "benchmark_frame.h"
#include <chrono>
#include <deque>
#include <string>

class benchmark {
public:
    benchmark();
    ~benchmark();

    benchmark_frame frame_starting();
    void frame_finished(benchmark_frame &frame);

    std::string get_stats() const;

private:
    static const unsigned max_history_length = 60;
    unsigned frames_created = 0;
    std::deque<benchmark_frame> frame_history;
    benchmark_frame history_sum;
};

#endif
