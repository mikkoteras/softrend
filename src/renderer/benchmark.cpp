#include "benchmark.h"
#include <iomanip>
#include <sstream>

using namespace std;
using namespace std::chrono;

typedef benchmark::timestamp_t timestamp_t;

benchmark::benchmark() {
}

benchmark::~benchmark() {
}

timestamp_t benchmark::frame_starting() {
    auto entered = high_resolution_clock::now();

    frame_in_progress = frame();
    frame_in_progress.frame_number = frames_created;
    frame_in_progress.frame_started = entered;
    frames_created += 1;

    return entered;
}

void benchmark::frame_finished(timestamp_t start_timestamp) {
    auto entered = high_resolution_clock::now();
    frame_in_progress.frame_finished = entered;

    if (frame_history.size() >= max_history_length) {
        history_sum -= frame_history.front();
        frame_history.pop_front();
    }

    history_sum += frame_in_progress;
    frame_history.push_back(frame_in_progress);
}

timestamp_t benchmark::clear_starting() {
    return high_resolution_clock::now();
}

void benchmark::clear_finished(timestamp_t start_timestamp) {
    frame_in_progress.clear_time += duration<double, milli>(high_resolution_clock::now() - start_timestamp).count();
}

timestamp_t benchmark::compute_starting() {
    return high_resolution_clock::now();
}

void benchmark::compute_finished(timestamp_t start_timestamp) {
    frame_in_progress.compute_time += duration<double, milli>(high_resolution_clock::now() - start_timestamp).count();
}

timestamp_t benchmark::render_starting() {
    return high_resolution_clock::now();
}

void benchmark::render_finished(timestamp_t start_timestamp) {
    frame_in_progress.render_time += duration<double, milli>(high_resolution_clock::now() - start_timestamp).count();
}

timestamp_t benchmark::copy_starting() {
    return high_resolution_clock::now();
}

void benchmark::copy_finished(timestamp_t start_timestamp) {
    frame_in_progress.copy_time += duration<double, milli>(high_resolution_clock::now() - start_timestamp).count();
}

string benchmark::get_stats() const {
    if (frame_history.empty())
        return "no benchmark data";

    int frames = frame_history.size();
    double total_time = duration<double, milli>(frame_history.back().frame_finished -
                                                frame_history.front().frame_started).count();
    double frame_time = total_time / frames;
    double fps = 1000 * frames / total_time;
    double clearing = history_sum.clear_time / frame_history.size();
    double computing = history_sum.compute_time / frame_history.size();
    double rendering = history_sum.render_time / frame_history.size();
    double copying = history_sum.copy_time / frame_history.size();

    ostringstream result;
    result << fixed << setw(6) << setprecision(2)
           << "frame time: " << frame_time
           << " ms | fps: " << fps
           << " | clear: " << clearing
           << " ms | compute: " << computing
           << " ms | render: " << rendering
           << " ms | copying: " << copying << " ms";
    return result.str();
}

const benchmark::frame &benchmark::frame::operator+=(const frame &rhs) {
    clear_time += rhs.clear_time;
    compute_time += rhs.compute_time;
    render_time += rhs.render_time;
    copy_time += rhs.copy_time;
    return *this;
}

const benchmark::frame &benchmark::frame::operator-=(const frame &rhs) {
    clear_time -= rhs.clear_time;
    compute_time -= rhs.compute_time;
    render_time -= rhs.render_time;
    copy_time -= rhs.copy_time;
    return *this;
}
