#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>
#include <deque>
#include <string>
#include <vector>

class benchmark {
public:
    typedef std::chrono::high_resolution_clock::time_point timestamp_t;

    benchmark();
    ~benchmark();

    timestamp_t frame_starting();
    void frame_finished(timestamp_t start_timestamp);
    timestamp_t clear_starting();
    void clear_finished(timestamp_t start_timestamp);
    timestamp_t compute_starting();
    void compute_finished(timestamp_t start_timestamp);
    timestamp_t render_starting();
    void render_finished(timestamp_t start_timestamp);
    timestamp_t copy_starting();
    void copy_finished(timestamp_t start_timestamp);

    std::string get_stats() const;

private:
    struct frame {
        unsigned frame_number;
        timestamp_t frame_started;
        timestamp_t frame_finished;
        double clear_time = 0.0f;
        double compute_time = 0.0f;
        double render_time = 0.0f;
        double copy_time = 0.0f;
        const frame &operator+=(const frame &rhs);
        const frame &operator-=(const frame &rhs);
    };
    
private:
    const unsigned max_history_length = 60;
    unsigned frames_created = 0;
    std::deque<frame> frame_history;
    frame frame_in_progress;
    frame history_sum;
};

#endif
