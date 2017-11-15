#include "sandbox.h"
#include "ambient_light.h"
#include "color.h"
#include "importer.h"
#include "vector.h"

using namespace math;
using namespace std;

sandbox::sandbox() {
    set_eye_position(vector3f{0, 0, 8});
    set_eye_reference_point(vector3f{0, 0, 0});
    set_eye_orientation(vector3f{0, 1, 0});
    set_view_to_view_plane_distance(2);

    icosahedron = importer::load_3dmax_object("assets/icosa/icosa.obj", materials(), true);
    
    icosahedron.set_scaling(1, 1, 1);
    icosahedron.set_position(-2, -2, -2);

    add_ambient_light(color(1.0f, 1.0f, 1.0f, 1.0f));
}

sandbox::~sandbox() {
}

void sandbox::render(framebuffer &fb) {
    float t = clock.seconds();

    set_eye_position(vector3f{15.f * cosf(t / 15.0f), 5.0f, 15.0f * sinf(t / 15.0)});
    set_eye_reference_point(vector3f{0, 0, 0});
    set_eye_orientation(vector3f{0, 1, 0});

    cs.render(*this, fb);
    icosahedron.render(*this, fb);
}
