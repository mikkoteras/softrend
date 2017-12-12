#include "benchmark.h"
#include <iomanip>
#include <sstream>

using namespace std;
using namespace std::chrono;

benchmark::benchmark() {
}

benchmark::~benchmark() {
}

benchmark_frame benchmark::frame_starting() {
    return benchmark_frame(frames_created++, high_resolution_clock::now());
}

void benchmark::frame_finished(benchmark_frame &frame) {
    frame.finish();
    history_sum += frame;
    frame_history.push_back(frame);
    
    while (frame_history.size() > max_history_length) {
        history_sum -= frame_history.front();
        frame_history.pop_front();
    }
}

string benchmark::get_stats() const {
    int frames = frame_history.size();

    if (frames < 2)
        return "no benchmark data";
    
    double avg_frame_time = frame_history.front().average_frame_time(frame_history.back());
    double fps = frame_history.front().average_frames_per_second(frame_history.back());
    double clearing = history_sum.time_spent_clearing() / frames;
    double computing = history_sum.time_spent_computing() / frames;
    double rendering = history_sum.time_spent_rendering() / frames;
    double copying = history_sum.time_spent_copying() / frames;

    ostringstream result;
    result << fixed << setw(6) << setprecision(2)
           << "frame time: " << avg_frame_time
           << " ms | fps: " << fps
           << " | clear: " << clearing
           << " ms | compute: " << computing
           << " ms | render: " << rendering
           << " ms | copying: " << copying << " ms";
    return result.str();
}
