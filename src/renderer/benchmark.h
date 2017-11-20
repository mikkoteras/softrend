#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>
#include <deque>

class benchmark {
public:
    benchmark();
    ~benchmark();

    void update_starting();
    void clear_finished();
    void render_finished();
    void copy_finished();
    
private:
    typedef std::chrono::high_resolution_clock::time_point tp;

    bool first_frame;
    
    int frames_rendered;
    tp previous_update_start;
    tp previous_time_point;

    double total_time_spent;
    double time_spent_clearing;
    double time_spent_rendering;
    double time_spent_copying;
};

#endif
