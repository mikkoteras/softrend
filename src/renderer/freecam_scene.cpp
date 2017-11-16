#include "freecam_scene.h"
#include "vector.h"
#include "SDL.h"

using namespace math;

freecam_scene::freecam_scene() :
    show_coords(false),
    coords(color(0.7f, 0.6f, 0.6f, 1.0f), color(0.6f, 0.7f, 0.6f, 1.0f), color(0.6f, 0.6f, 0.7f, 1.0f)) {
}

freecam_scene::~freecam_scene() {
}

void freecam_scene::render(framebuffer &fb) {
    if (show_coords)
        coords.render(*this, fb);
}

void freecam_scene::key_down_event(int sdl_keycode, bool ctrl_is_down) {
    if (ctrl_is_down) {
        if (sdl_keycode == SDLK_c)
            show_coords = !show_coords;
    }
    else {
        if (sdl_keycode == SDLK_w)
            set_eye_position(get_eye_position() - vector3f{0.0f, 0.0f, 0.2f});
        else if (sdl_keycode == SDLK_a)
            set_eye_position(get_eye_position() - vector3f{0.2f, 0.0f, 0.0f});
        else if (sdl_keycode == SDLK_s)
            set_eye_position(get_eye_position() + vector3f{0.0f, 0.0f, 0.2f});
        else if (sdl_keycode == SDLK_d)
            set_eye_position(get_eye_position() + vector3f{0.2f, 0.0f, 0.0f});
    }
}

void freecam_scene::mouse_move_event(int delta_x, int delta_y) {
}

void freecam_scene::mouse_wheel_event(int delta) {
}
