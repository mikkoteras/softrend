#include "alpha_test.h"
#include "framebuffer.h"
#include "importer.h"
#include "directional_light.h"
#include "vector.h"

using namespace math;

alpha_test::alpha_test(bool echo_comments) :
    scene() {

    try {
        blue_icosa = importer::load_wavefront_object("blue_icosa.obj", materials(), echo_comments);
    }
    catch (importer::importer_exception) {
        stop();
        return;
    }

    light_sources().set_ambient_light(color(0.5f, 0.5f, 0.5f, 0.5f));
    light_sources().add_light(directional_light(vector3f{0.0f, -1.0f, 0.0f}, color(0.5f, 0.5f, 0.5f, 0.5f)));
    
    blue_icosa.set_position(0, 0, 0);

    set_eye_position(vector3f{0.0f, 0.0f, 5.0f});
    set_eye_reference_point(vector3f{0.0f, 0.0f, 0.0f});
    set_eye_orientation(vector3f{0.0f, 1.0f, 0.0f});

    set_view_to_view_plane_distance(2.0f);
}

alpha_test::~alpha_test() {
}

void alpha_test::render(framebuffer &fb) {
    float t = clock.seconds();

    blue_icosa.set_rotation(0, t, t / 10.0f);
    
    blue_icosa.render(*this, fb, visualize_normals, visualize_reflection_vectors);
}
