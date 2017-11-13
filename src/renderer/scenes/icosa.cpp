#include "icosa.h"
#include "importer.h"
#include "vector.h"

using namespace math;

icosa::icosa() {
    icosahedron = importer::load_3dmax_object("objects/icosa/icosa.obj", materials(), true);
    
    icosahedron.set_scaling(1, 1, 1);
    
    set_eye_position(vector3f{0, 0, 8});
    set_eye_reference_point(vector3f{0, 0, 0});
    set_view_to_view_plane_distance(2);
}

icosa::~icosa() {
}

void icosa::render(framebuffer &fb) {
    float t = clock.seconds();

    icosahedron.set_rotation(0, 3.f * t, 0.05f * t);
    icosahedron.set_position(sinf(t), .2f * sinf(2.f * t), sinf(1.2f * t));

    icosahedron.render(*this, fb);    
}
