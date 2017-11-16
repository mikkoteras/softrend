#include "auto_scene.h"
#include "importer.h"
#include "vector.h"

using namespace math;
using namespace std;

icosa::icosa() {
    icosahedron = importer::load_3dmax_object("assets/icosa/icosa.obj", materials(), true);
    
    icosahedron.set_scaling(1, 1, 1);
    
    set_eye_position(vector3f{0, 0, 8});
    set_eye_reference_point(vector3f{0, 0, 0});
    set_view_to_view_plane_distance(2);

    add_ambient_light(color(0.2f, 0.2f, 0.2f, 1.0f));
    add_directional_light(vector3f{1, 0, 0}, color(0.8f, 0.2f, 0.2f, 1.0f));
    add_directional_light(vector3f{-1, 0, 0}, color(0.2f, 0.8f, 0.2f, 1.0f));
    add_directional_light(vector3f{0, 1, -1}, color(0.2f, 0.2f, 0.8f, 1.0f));
}

icosa::~icosa() {
}

void icosa::render(framebuffer &fb) {
    float t = clock.seconds();

    icosahedron.set_rotation(0, 0.3f * t, 0.1f * t);
    //icosahedron.set_position(sinf(0.1f * t), 0.2f * sinf(2.f * t), sinf(0.12f * t));

    icosahedron.render(*this, fb);    
}
