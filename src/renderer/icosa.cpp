#include "icosa.h"
#include "importer.h"
#include "vector.h"

using namespace math;
using namespace std;

icosa::icosa() :
    freecam_scene() {
    
    icosahedron = importer::load_3dmax_object("assets/icosa/icosa.obj", materials(), true);
    icosahedron.set_scaling(1, 1, 1);
    
    add_ambient_light(color(0.2f, 0.2f, 0.2f, 1.0f));
    add_directional_light(vector3f{1, 0, 0}, color(0.8f, 0.2f, 0.2f, 1.0f));
    add_directional_light(vector3f{-1, 0, 0}, color(0.2f, 0.8f, 0.2f, 1.0f));
    add_directional_light(vector3f{0, 1, -1}, color(0.2f, 0.2f, 0.8f, 1.0f));
}

icosa::~icosa() {
}

void icosa::render(framebuffer &fb) {
    freecam_scene::render(fb);
    icosahedron.render(*this, fb);    
}
