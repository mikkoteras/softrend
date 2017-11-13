#include "dnahelix.h"
#include "framebuffer.h"
#include "importer.h"
#include "vector.h"
#include <cmath>

using namespace math;

dnahelix::dnahelix() {
    dnahelix_mesh = importer::load_3dmax_object("assets/dnahelix/DNA.obj", materials(), true);
    
    dnahelix_mesh.set_scaling(1.f, 1.f, 1.f);
    dnahelix_mesh.set_position(0, 0, 0);

    set_view_to_view_plane_distance(2);
}

dnahelix::~dnahelix() {
}

void dnahelix::render(framebuffer &fb) {
    float t = clock.seconds();

    dnahelix_mesh.set_rotation(0.f, 0.f, 0.5f * t);
    
    set_eye_position(vector3f{0, 0, 16});
    set_eye_reference_point(vector3f{0, 0, 0});
    set_eye_orientation(vector3f{0, 1, 0});
    
    if (t > .1)
        dnahelix_mesh.render(*this, fb);
}
