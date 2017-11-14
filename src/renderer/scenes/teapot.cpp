#include "teapot.h"
#include "framebuffer.h"
#include "importer.h"
#include "vector.h"
#include <cmath>

using namespace math;

teapot::teapot() {
    teapot_mesh = importer::load_3dmax_object("assets/teapot/tpot.obj", materials(), true);
    
    teapot_mesh.set_scaling(.3f, .3f, .3f);
    teapot_mesh.set_position(0, 0, 0);

    set_eye_position(vector3f{0.f, 2.0f, 15.f});
    set_eye_reference_point(vector3f{0, 0, 0});
    set_eye_orientation(vector3f{0, 1, 0});
    set_view_to_view_plane_distance(2);

    add_light(directional_light(vector3f{-1, 0, 0}, color(0.8f, 0.1f, 0.1f, 1.0f)));
    add_light(directional_light(vector3f{1, 0, 0}, color(0.1f, 0.8f, 0.1f, 1.0f)));
    add_light(directional_light(vector3f{0, 1, -1}, color(0.1f, 0.1f, 0.8f, 1.0f)));
}

teapot::~teapot() {
}

void teapot::render(framebuffer &fb) {
    float t = clock.seconds();

    teapot_mesh.set_rotation(0.f, 0.2f * t, 0.3f * sinf(.4f * t));

    if (t > .1)
        teapot_mesh.render(*this, fb);
}
