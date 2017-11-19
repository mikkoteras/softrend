#include "auto_scene.h"
#include "importer.h"
#include "vector.h"
#include <iostream>

using namespace math;
using namespace std;
using namespace std::experimental::filesystem;

auto_scene::auto_scene(const path &object_file, bool echo_comments, object_position pos) :
    freecam_scene(5.0) {
    
    object = importer::load_3dmax_object(object_file, materials(), echo_comments);
    add_ambient_light(color(1.0f, 1.0f, 1.0f, 1.0f));

    bounding_box box = object.local_bounding_box();
    float max_semiaxis = box.max_semiaxis();

    if (max_semiaxis > 0.0f) {
        float scale = 1.0f / max_semiaxis;
        object.set_scaling(scale, scale, scale);
        box /= max_semiaxis;

        if (pos == center_at_origin)
            object.set_position(-box.min().x() - box.width() / 2.0f,
                                -box.min().y() - box.height() / 2.0f,
                                -box.min().z() - box.depth() / 2.0f);
        else if (pos == bounding_box_touches_origin)
            object.set_position(-box.min().x(),
                                -box.min().y(),
                                -box.min().z());
        else if (pos == center_at_unity)
            object.set_position(1.0f - box.min().x() - box.width() / 2.0f,
                                1.0f - box.min().y() - box.height() / 2.0f,
                                1.0f - box.min().z() - box.depth() / 2.0f);
    }
}

auto_scene::~auto_scene() {
}

void auto_scene::render(framebuffer &fb) {
    freecam_scene::render(fb);
    object.render(*this, fb);    
}
