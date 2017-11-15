#include "sandbox.h"
#include "ambient_light.h"
#include "color.h"
#include "importer.h"
#include "vector.h"

using namespace math;
using namespace std;

sandbox::sandbox() {
    line_thing.set_scaling(1, 1, 1);
    
    set_eye_position(vector3f{0, 0, 8});
    set_eye_reference_point(vector3f{0, 0, 0});
    set_eye_orientation(vector3f{0, 1, 0});
    set_view_to_view_plane_distance(2);

    line_thing.add_line(vector4f{-1, 0, 0, 1}, vector4f{1, 0, 0, 1},
                  color(0, 1, 1, 1), color(1, 0, 1, 1));
    line_thing.add_line(vector4f{0, -1, 0, 1}, vector4f{0, 1, 0, 1},
                  color(1, 1, 0, 1), color(1, 0, 0, 1));
    line_thing.add_line(vector4f{0, 0, -1, 1}, vector4f{0, 0, 1, 1},
                  color(0, 1, 0, 1), color(0, 0, 1, 1));

    icosahedron = importer::load_3dmax_object("assets/icosa/icosa.obj", materials(), true);
    
    icosahedron.set_scaling(1, 1, 1);
    icosahedron.set_position(-2, -2, -2);

    add_ambient_light(color(1.0f, 1.0f, 1.0f, 1.0f));
}

sandbox::~sandbox() {
}

void sandbox::render(framebuffer &fb) {
    float t = clock.seconds();

    line_thing.set_position(1, 1, 1);
    line_thing.set_rotation(0.1 * t, 0.7f * t, 1.1f * t);
    set_eye_position(vector3f{15.f * cosf(t / 15.0f), 5.0f, 15.0f * sinf(t / 15.0)});
    set_eye_reference_point(vector3f{0, 0, 0});
    set_eye_orientation(vector3f{0, 1, 0});

    cs.render(*this, fb);
    line_thing.render(*this, fb);
    icosahedron.render(*this, fb);
}
