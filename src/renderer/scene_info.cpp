#include "scene_info.h"
#include "scene.h"
#include "util.h"
#include "benchmark.h"
#include <iomanip>
#include <sstream>

using namespace std;

scene_info::scene_info() {
}

scene_info::~scene_info() {
}

void scene_info::update(scene &parent) {
    benchmark_stats = parent.get_benchmark().get_stats();

    ostringstream o;
    o << fixed << setw(6) << setprecision(2)
      << "animation time: " << parent.get_animation_time()
      << " | shading model: " << util::to_string(parent.get_shading_model())
      << " | normals: " << (parent.get_normal_visualization() ? "on" : "off")
      << " | reflections: " << (parent.get_reflection_vector_visualization() ? "on" : "off")
      << " | wireframe: " << (parent.get_wireframe_visualization() ? "on" : "off");
    render_stats = o.str();
}

vector<string> scene_info::get() const {
    vector<string> result;
    result.push_back(benchmark_stats);
    result.push_back(render_stats);
    return result;
}
