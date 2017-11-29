#include "scene_info.h"
#include "scene.h"
#include "benchmark.h"

using namespace std;

scene_info::scene_info() {
}

scene_info::~scene_info() {
}

void scene_info::update_scene_stats(const scene &) {
}

void scene_info::update_benchmark_stats(const benchmark &b) {
    benchmark_stats = b.get_stats();
}

void scene_info::update_render_info(const scene &) {
}

vector<string> scene_info::get() const {
    vector<string> result;
    result.push_back(benchmark_stats);
    return result;
}
