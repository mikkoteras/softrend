#include "touareg.h"
#include "importer.h"
#include "vector.h"

using namespace math;

touareg::touareg() {
    car = importer::load_3dmax_object("assets/touareg/Touareg.obj", materials(), false);

    car.set_scaling(.02f, .02f, .02f);
    car.set_position(0, -1, 0);

    set_view_to_view_plane_distance(3);
}

touareg::~touareg() {
}

void touareg::render(framebuffer &fb) {
    float t = clock.seconds();

    car.set_rotation(0, .1f * t, 0);

    set_eye_position(vector3f{0, 10, 150});
    set_eye_reference_point(vector3f{0, 0, 0});
    set_eye_orientation(vector3f{0, 1, 0});

    if (t > .1f)
        car.render(*this, fb);
}
