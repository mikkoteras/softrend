#include "coordinate_system.h"
#include "color.h"
#include "vector.h"

using namespace math;

coordinate_system::coordinate_system(scene *parent_scene) :
    coordinate_system(parent_scene,
                      color(0.8f, 0.2f, 0.2f, 1.0f),
                      color(0.2f, 0.8f, 0.2f, 1.0f),
                      color(0.2f, 0.2f, 0.8f, 1.0f)) {
}

coordinate_system::coordinate_system(scene *parent_scene, const color &x,const color &y, const color &z) :
    m(parent_scene) {

    const float t = 6; // ticks

    // Axes
    m.add_line(vector3f{-t, 0.0f, 0.0f}, vector3f{t, 0.0f, 0.0f}, x, x);
    m.add_line(vector3f{0.0f, -t, 0.0f}, vector3f{0.0f, t, 0.0f}, y, y);
    m.add_line(vector3f{0.0f, 0.0f, -t}, vector3f{0.0f, 0.0f, t}, z, z);

    // Axis ticks
    for (float i = 1.0f; i < t + 0.5f; i += 1.0f) {
        m.add_line(vector3f{i, -0.05f, 0.0f}, vector3f{i, 0.05f, 0.0f}, x, x);
        m.add_line(vector3f{-i, -0.05f, 0.0f}, vector3f{-i, 0.05f, 0.0f}, x, x);
        m.add_line(vector3f{-0.05f, i, 0.0f}, vector3f{0.05f, i, 0.0f}, y, y);
        m.add_line(vector3f{-0.05f, -i, 0.0f}, vector3f{0.05f, -i, 0.0f}, y, y);
        m.add_line(vector3f{-0.05f, 0.0f, i}, vector3f{0.05f, 0.0f, i}, z, z);
        m.add_line(vector3f{-0.05f, 0.0f, -i}, vector3f{0.05f, 0.0f, -i}, z, z);
    }

    // Axis labels
    m.add_line(vector3f{4.95f, -0.3f, 0.0f}, vector3f{5.05f, -0.5f, 0.0f}, x, x); // x
    m.add_line(vector3f{5.05f, -0.3f, 0.0f}, vector3f{4.95f, -0.5f, 0.0f}, x, x);
    m.add_line(vector3f{-5.2f, -0.4f, 0.0f}, vector3f{-5.1f, -0.4f, 0.0f}, x, x); // -x
    m.add_line(vector3f{-4.95f, -0.3f, 0.0f}, vector3f{-5.05f, -0.5f, 0.0f}, x, x);
    m.add_line(vector3f{-5.05f, -0.3f, 0.0f}, vector3f{-4.95f, -0.5f, 0.0f}, x, x);
    m.add_line(vector3f{.4f, 5.1f, 0.0f}, vector3f{.3f, 4.9f, 0.0f}, y, y); // y
    m.add_line(vector3f{.3f, 5.1f, 0.0f}, vector3f{.35f, 5.0f, 0.0f}, y, y);
    m.add_line(vector3f{.2f, -5.0f, 0.0f}, vector3f{.3f, -5.0f, 0.0f}, y, y); // -y
    m.add_line(vector3f{.3f, -4.9f, 0.0f}, vector3f{.35f, -5.0f, 0.0f}, y, y);
    m.add_line(vector3f{.3f, -5.1f, 0.0f}, vector3f{.4f, -4.9f, 0.0f}, y, y);
    m.add_line(vector3f{.3f, 0.1f, 5.0f}, vector3f{.4f, 0.1f, 5.0f}, z, z); // z
    m.add_line(vector3f{.4f, 0.1f, 5.0f}, vector3f{.3f, -0.1f, 5.0f}, z, z);
    m.add_line(vector3f{.3f, -0.1f, 5.0f}, vector3f{.4f, -0.1f, 5.0f}, z, z);
    m.add_line(vector3f{.2f, 0.0f, -5.0f}, vector3f{.3f, 0.0f, -5.0f}, z, z); // -z
    m.add_line(vector3f{.3f, 0.1f, -5.0f}, vector3f{.4f, 0.1f, -5.0f}, z, z);
    m.add_line(vector3f{.4f, 0.1f, -5.0f}, vector3f{.3f, -0.1f, -5.0f}, z, z);
    m.add_line(vector3f{.3f, -0.1f, -5.0f}, vector3f{.4f, -0.1f, -5.0f}, z, z);
}

coordinate_system::~coordinate_system() {
}
