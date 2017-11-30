#include "scene_info.h"
#include "scene.h"
#include "benchmark.h"

using namespace std;

scene_info::scene_info() {
}

scene_info::~scene_info() {
}

void scene_info::update(scene &parent) {
    benchmark_stats = parent.get_benchmark().get_stats();
}

vector<string> scene_info::get() const {
    vector<string> result;
    result.push_back(benchmark_stats);
    return result;
}
