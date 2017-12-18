#include "auto_scene.h"
#include "directional_light.h"
#include "importer.h"
#include "point_light.h"
#include "vector.h"
#include "SDL.h"

using namespace math;
using namespace std;
using namespace std::experimental::filesystem;

auto_scene::auto_scene(const path &object_file, bool echo_comments, object_position pos) :
    freecam_scene(10.0f) {

    try {
        object = importer::load_wavefront_object(object_file, materials(), echo_comments);
        toggle_lights();
        
        bounding_box box = object.local_bounding_box();
        float max_semiaxis = box.max_semiaxis();

        if (max_semiaxis > 0.0f) {
            float scale = 1.0f / max_semiaxis;
            object.set_scaling(scale, scale, scale);
            box /= max_semiaxis;

            if (pos == as_is)
                point_of_interest(vector3f{0.0f, 0.0f, 0.0f});
            else if (pos == center_at_origin) {
                object.set_position(-box.min().x() - box.width() / 2.0f,
                                    -box.min().y() - box.height() / 2.0f,
                                    -box.min().z() - box.depth() / 2.0f);
                point_of_interest(vector3f{0.0f, 0.0f, 0.0f});
            }
            else if (pos == bounding_box_touches_origin) {
                object.set_position(-box.min().x(),
                                    -box.min().y(),
                                    -box.min().z());
                point_of_interest(vector3f{box.width() / 2.0f, box.height() / 2.0f, box.depth() / 2.0f});
            }
            else if (pos == center_at_unity) {
                object.set_position(1.0f - box.min().x() - box.width() / 2.0f,
                                    1.0f - box.min().y() - box.height() / 2.0f,
                                    1.0f - box.min().z() - box.depth() / 2.0f);
                point_of_interest(vector3f{1.0f, 1.0f, 1.0f});
            }
        }
    }
    catch (importer::importer_exception) {
        stop();
    }
}

auto_scene::~auto_scene() {
}

void auto_scene::key_down_event(int sdl_keycode, bool ctrl_is_down) {
    if (sdl_keycode == SDLK_KP_7) {
        x_rotation_per_second -= 0.1f;
        return;
    }
    else if (sdl_keycode == SDLK_KP_8) {
        x_rotation_per_second += 0.1f;
        return;
    }
    else if (sdl_keycode == SDLK_KP_4) {
        y_rotation_per_second -= 0.1f;
        return;
    }
    else if (sdl_keycode == SDLK_KP_5) {
        y_rotation_per_second += 0.1f;
        return;
    }
    else if (sdl_keycode == SDLK_KP_1) {
        z_rotation_per_second -= 0.1f;
        return;
    }
    else if (sdl_keycode == SDLK_KP_2) {
        z_rotation_per_second += 0.1f;
        return;
    }
    else if (ctrl_is_down && sdl_keycode == SDLK_l)
        toggle_lights();
    else if (ctrl_is_down && sdl_keycode == SDLK_r) {
        x_rotation = 0.0f;
        y_rotation = 0.0f;
        z_rotation = 0.0f;
        x_rotation_per_second = 0.0f;
        y_rotation_per_second = 0.0f;
        z_rotation_per_second = 0.0f;
    }

    freecam_scene::key_down_event(sdl_keycode, ctrl_is_down);
}

void auto_scene::toggle_lights() {
    light_list &lights = light_sources();
    lights.clear();
    lights_on = !lights_on;

    if (lights_on) {
        lights.add_light(directional_light(vector3f{-0.1f, -1.0f, 0.0f}, color(1.0f, 1.0f, 1.0f, 1.0f)));
        lights.set_ambient_light(color(0.2f, 0.2f, 0.2f, 1.0f));
    }
    else
        lights.set_ambient_light(color(1.0f, 1.0f, 1.0f, 1.0f));
}

void auto_scene::render(framebuffer &fb) {
    float t = clock.seconds();
    float delta_t = t - previous_render_time;
    previous_render_time = t;

    x_rotation += delta_t * x_rotation_per_second;
    y_rotation += delta_t * y_rotation_per_second;
    z_rotation += delta_t * z_rotation_per_second;
    object.set_rotation(x_rotation, y_rotation, z_rotation);
    
    freecam_scene::render(fb);
    object.render(*this, fb, visualize_normals, visualize_reflection_vectors);
}
