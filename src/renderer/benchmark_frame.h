#ifndef BENCHMARK_FRAME_H
#define BENCHMARK_FRAME_H

#include <chrono>

class benchmark_frame {
public:
    typedef std::chrono::high_resolution_clock::time_point timestamp_t;
    typedef unsigned frame_number_t;

    benchmark_frame();
    benchmark_frame(frame_number_t frame_number, timestamp_t frame_started);
    ~benchmark_frame();

    frame_number_t get_frame_number() const;
    
    timestamp_t clear_starting();
    void clear_finished(timestamp_t start_timestamp);
    timestamp_t render_starting();
    void render_finished(timestamp_t start_timestamp);
    timestamp_t convert_starting();
    void convert_finished(timestamp_t start_timestamp);
    timestamp_t copy_starting();
    void copy_finished(timestamp_t start_timestamp);
    void finish();

    double time_spent_clearing() const;
    double time_spent_rendering() const;
    double time_spent_converting() const;
    double time_spent_copying() const;

    double average_frame_time(const benchmark_frame &rhs) const;
    double average_frames_per_second(const benchmark_frame &rhs) const;
    
    const benchmark_frame &operator+=(const benchmark_frame &rhs);
    const benchmark_frame &operator-=(const benchmark_frame &rhs);
    
private:
    frame_number_t frame_number;
    timestamp_t frame_started;
    timestamp_t frame_finished;
    double clear_time = 0.0;
    double render_time = 0.0;
    double convert_time = 0.0;
    double copy_time = 0.0;
};

#endif
