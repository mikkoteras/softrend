#include "benchmark_frame.h"

using namespace std;
using namespace std::chrono;

benchmark_frame::benchmark_frame() :
    frame_number(-1) {
}

benchmark_frame::benchmark_frame(frame_number_t frame_number, timestamp_t frame_started) :
    frame_number(frame_number),
    frame_started(frame_started) {
}

benchmark_frame::~benchmark_frame() {
}

benchmark_frame::frame_number_t benchmark_frame::get_frame_number() const {
    return frame_number;
}

benchmark_frame::timestamp_t benchmark_frame::clear_starting() {
    return high_resolution_clock::now();
}

void benchmark_frame::clear_finished(timestamp_t start_timestamp) {
    clear_time += duration<double, milli>(high_resolution_clock::now() - start_timestamp).count();
}

benchmark_frame::timestamp_t benchmark_frame::render_starting() {
    return high_resolution_clock::now();
}

void benchmark_frame::render_finished(timestamp_t start_timestamp) {
    render_time += duration<double, milli>(high_resolution_clock::now() - start_timestamp).count();
}

benchmark_frame::timestamp_t benchmark_frame::convert_starting() {
    return high_resolution_clock::now();
}

void benchmark_frame::convert_finished(timestamp_t start_timestamp) {
    convert_time += duration<double, milli>(high_resolution_clock::now() - start_timestamp).count();
}

benchmark_frame::timestamp_t benchmark_frame::copy_starting() {
    return high_resolution_clock::now();
}

void benchmark_frame::copy_finished(timestamp_t start_timestamp) {
    copy_time += duration<double, milli>(high_resolution_clock::now() - start_timestamp).count();
}

void benchmark_frame::finish() {
    frame_finished = high_resolution_clock::now();
}

double benchmark_frame::time_spent_clearing() const {
    return clear_time;
}

double benchmark_frame::time_spent_rendering() const {
    return render_time;
}

double benchmark_frame::time_spent_converting() const {
    return convert_time;
}

double benchmark_frame::time_spent_copying() const {
    return copy_time;
}

double benchmark_frame::average_frame_time(const benchmark_frame &rhs) const {
    if (frame_number > rhs.frame_number - 1)
        return 0.0;

    return duration<double, milli>(rhs.frame_finished - frame_finished).count() / (rhs.frame_number - frame_number);
        
}

double benchmark_frame::average_frames_per_second(const benchmark_frame &rhs) const {
    if (frame_number > rhs.frame_number - 1)
        return 0.0;

    return (rhs.frame_number - frame_number) / duration<double>(rhs.frame_finished - frame_finished).count();
}

const benchmark_frame &benchmark_frame::operator+=(const benchmark_frame &rhs) {
    clear_time += rhs.clear_time;
    convert_time += rhs.convert_time;
    render_time += rhs.render_time;
    copy_time += rhs.copy_time;
    return *this;
}

const benchmark_frame &benchmark_frame::operator-=(const benchmark_frame &rhs) {
    clear_time -= rhs.clear_time;
    convert_time -= rhs.convert_time;
    render_time -= rhs.render_time;
    copy_time -= rhs.copy_time;
    return *this;
}
