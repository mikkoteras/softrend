#include "alpha_test.h"
#include "directional_light.h"
#include "framebuffer.h"
#include "importer.h"
#include "line.h"
#include "math_detail.h"
#include "vector.h"

using namespace math;

alpha_test::alpha_test(bool verbose) :
    scene(),
    backdrop(this),
    red_icosa(this),
    green_icosa(this),
    blue_icosa(this) {

    try {
        importer::load_wavefront_object(backdrop, "backdrop.obj", materials(), verbose);
        importer::load_wavefront_object(red_icosa, "red_icosa.obj", materials(), verbose);
        importer::load_wavefront_object(green_icosa, "green_icosa.obj", materials(), verbose);
        importer::load_wavefront_object(blue_icosa, "blue_icosa.obj", materials(), verbose);
    }
    catch (importer::importer_exception) {
        stop();
        return;
    }

    light_sources().set_ambient_light(color(0.2f, 0.2f, 0.2f, 1.0f));
    light_sources().add_light(directional_light(vector3f{0.0f, -1.0f, 0.0f}, color(0.5f, 0.5f, 0.5f, 1.0f)));

    set_eye_position(vector3f{0.0f, 0.0f, 5.0f});
    set_eye_reference_point(vector3f{0.0f, 0.0f, 0.0f});
    set_eye_orientation(vector3f{0.0f, 1.0f, 0.0f});
    set_fov((120.0f / 360.0) * (2.0f * math::detail::pi<float>()));
}

alpha_test::~alpha_test() {
}

void alpha_test::compose() {
    using namespace math::detail;

    float t = clock.seconds();

    backdrop.set_scaling(6.0f, 6.0f, 6.0f);
    backdrop.set_position(0, 0, -10);

    red_icosa.set_rotation(0, t, t / 10.0f);
    red_icosa.set_position(3.0f * cos<float>(t),
                           0,
                           -1.5f * sin<float>(t));
    green_icosa.set_rotation(0, t, t / 10.0f);
    green_icosa.set_position(3.0f * cos<float>(t + 2.0f * pi<float>() / 3.0f),
                             0,
                             -1.5f * sin<float>(t + 2.0f * pi<float>() / 3.0f));
    blue_icosa.set_rotation(0, t, t / 10.0f);
    blue_icosa.set_position(3.0f * cos<float>(t + 4.0f * pi<float>() / 3.0f),
                            0,
                            -1.5f * sin<float>(t + 4.0f * pi<float>() / 3.0f));
}
