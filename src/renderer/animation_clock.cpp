#include "animation_clock.h"

using namespace std;
using namespace std::chrono;

animation_clock::animation_clock() {
}

animation_clock::~animation_clock() {
}

void animation_clock::start() {
    start_time = steady_clock::now();
}

float animation_clock::seconds() const {
    return duration<float>(steady_clock::now() - start_time).count();
}
