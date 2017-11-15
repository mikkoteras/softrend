#include "coordinate_system.h"
#include "color.h"
#include "vector.h"

using namespace math;

coordinate_system::coordinate_system() :
    coordinate_system(color(0.8f, 0.2f, 0.2f, 1.0f),
                      color(0.2f, 0.8f, 0.2f, 1.0f),
                      color(0.2f, 0.2f, 0.8f, 1.0f)) {
}

coordinate_system::coordinate_system(const color &x,const color &y, const color &z) {

    const float t = 5; // ticks
    
    // Axes
    m.add_line(vector4f{-t, 0.0f, 0.0f, 1.0f}, vector4f{t, 0.0f, 0.0f, 1.0f}, x, x);
    m.add_line(vector4f{0.0f, -t, 0.0f, 1.0f}, vector4f{0.0f, t, 0.0f, 1.0f}, y, y);
    m.add_line(vector4f{0.0f, 0.0f, -t, 1.0f}, vector4f{0.0f, 0.0f, t, 1.0f}, z, z);

    // Axis ticks
    for (float i = 1.0f; i < t + 0.5f; i += 1.0f) {
        m.add_line(vector4f{i, -0.05f, 0.0f, 1.0f}, vector4f{i, 0.05f, 0.0f, 1.0f}, x, x);
        m.add_line(vector4f{-i, -0.05f, 0.0f, 1.0f}, vector4f{-i, 0.05f, 0.0f, 1.0f}, x, x);
        m.add_line(vector4f{-0.05, i, 0.0f, 1.0f}, vector4f{0.05f, i, 0.0f, 1.0f}, y, y);
        m.add_line(vector4f{-0.05, -i, 0.0f, 1.0f}, vector4f{0.05f, -i, 0.0f, 1.0f}, y, y);
        m.add_line(vector4f{-0.05, 0.0f, i, 1.0f}, vector4f{0.05f, 0.0f, i, 1.0f}, z, z);
        m.add_line(vector4f{-0.05, 0.0f, -i, 1.0f}, vector4f{0.05f, 0.0f, -i, 1.0f}, z, z);
    }

    // TODO: labels
}

coordinate_system::~coordinate_system() {
}

void coordinate_system::render(scene &sc, framebuffer &fb) {
    m.render(sc, fb);
}
