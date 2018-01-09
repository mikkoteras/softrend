#include "alpha_test.h"
#include "color3.h"
#include "command_line.h"
#include "directional_light.h"
#include "framebuffer.h"
#include "importer.h"
#include "line.h"
#include "math/math_detail.h"
#include "math/vector.h"

using namespace math;

alpha_test::alpha_test(const command_line &cl) :
    scene(cl),
    backdrop(this),
    red_icosa(this),
    green_icosa(this),
    blue_icosa(this) {

    try {
        importer::load_wavefront_object(backdrop, "backdrop.obj", materials(), cl.verbose());
        importer::load_wavefront_object(red_icosa, "red_icosa.obj", materials(), cl.verbose());
        importer::load_wavefront_object(green_icosa, "green_icosa.obj", materials(), cl.verbose());
        importer::load_wavefront_object(blue_icosa, "blue_icosa.obj", materials(), cl.verbose());
    }
    catch (importer::importer_exception) {
        stop();
        return;
    }

    light_sources().set_ambient_light(0x505050_rgb);
    light_sources().add_light(directional_light(vector3f{0.0f, -1.0f, 0.0f}, 0x808080_rgb));

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
