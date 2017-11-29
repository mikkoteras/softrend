#include "benchmark.h"
#include <iomanip>
#include <sstream>

using namespace std;
using namespace std::chrono;

benchmark::benchmark() :
    first_frame(false),
    frames_rendered(0),
    total_time_spent(0.0),
    time_spent_clearing(0.0),
    time_spent_rendering(0.0),
    time_spent_copying(0.0) {
}

benchmark::~benchmark() {
}

void benchmark::update_starting() {
    auto entered = high_resolution_clock::now();

    if (!first_frame)
        total_time_spent += duration<double, milli>(entered - previous_update_start).count();

    // TODO: how to clear this now?
    
    previous_update_start = entered;
    previous_time_point = high_resolution_clock::now();
}

void benchmark::clear_finished() {
    auto now = high_resolution_clock::now();
    time_spent_clearing += duration<double, milli>(now - previous_time_point).count();
    previous_time_point = now;
}

void benchmark::render_finished() {
    auto now = high_resolution_clock::now();
    time_spent_rendering += duration<double, milli>(now - previous_time_point).count();
    previous_time_point = now;
}

void benchmark::copy_finished() {
    ++frames_rendered;
    auto now = high_resolution_clock::now();
    time_spent_copying += duration<double, milli>(now - previous_time_point).count();
    previous_time_point = now;
}

string benchmark::get_stats() const {
    ostringstream result;
    result << fixed << setw(6) << setprecision(2)
           << " avg frame time = " << (total_time_spent / frames_rendered) << " ms "
           << " (" << (1000.0 * frames_rendered / total_time_spent) << " fps;"
           << " clear=" << (100.0 * time_spent_clearing / total_time_spent) << "%"
           << " render=" << (100.0 * time_spent_rendering / total_time_spent) << "%"
           << " copy_data=" << (100.0 * time_spent_copying / total_time_spent) << "%";
    return result.str();
}
