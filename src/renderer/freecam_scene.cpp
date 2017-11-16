#include "freecam_scene.h"
#include "linear_transforms.h"
#include "vector.h"
#include "SDL.h"

using namespace math;

freecam_scene::freecam_scene() :
    show_coords(false),
    coords(color(0.7f, 0.6f, 0.6f, 1.0f),
           color(0.6f, 0.7f, 0.6f, 1.0f),
           color(0.6f, 0.6f, 0.7f, 1.0f)),
    eye_azimuth_angle(0.0f),
    eye_zenith_angle(0.0f),
    eye_radius(10.0f),
    eye_twist_angle(0.0f) {
}

freecam_scene::~freecam_scene() {
}

void freecam_scene::render(framebuffer &fb) {
    if (show_coords)
        coords.render(*this, fb);
    /*
    float eye_x = eye_radius * cosf(eye_azimuth_angle) * sinf(eye_zenith_angle);
    float eye_y = eye_radius * sinf(eye_azimuth_angle) * sinf(eye_zenith_angle);
    float eye_z = eye_radius * cosf(eye_zenith_angle);

    set_eye_position(vector3f{eye_x, eye_y, eye_z});
    set_eye_reference_point(vector3f{0.0f, 0.0f, 0.0f});
    set_eye_orientation(vector3f{0.0f, 1.0f, 0.0f});
    */
}

void freecam_scene::key_down_event(int sdl_keycode, bool ctrl_is_down) {
    if (ctrl_is_down) {
        if (sdl_keycode == SDLK_c)
            show_coords = !show_coords;
        else if (sdl_keycode == SDLK_r) { // reset scene
            eye_azimuth_angle = 0.0f;
            eye_zenith_angle = 0.0f;
            eye_radius = 10.0f;
            eye_twist_angle = 0.0f;
        }
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

void freecam_scene::mouse_move_event(int delta_x, int delta_y, bool left_button_is_down) {
    if (left_button_is_down) {
        eye_azimuth_angle -= 0.005f * delta_x;
        eye_zenith_angle += 0.005f * delta_y;

        // TODO: dumb implementation
        while (eye_azimuth_angle < 0.0f)
            eye_azimuth_angle += 2 * M_PI;

        while (eye_azimuth_angle >= 2 * M_PI)
            eye_azimuth_angle -= 2 * M_PI;

        while (eye_zenith_angle < 0.0f)
            eye_zenith_angle += M_PI;

        while (eye_zenith_angle >= M_PI)
            eye_zenith_angle -= M_PI;

        //set_eye_position(vector3f{eye_x, eye_y, eye_z});
        
        vector4f eye = get_eye_position().homo();
        matrix4x4f x_rot = linear_transforms::rotate3x(0.001f * delta_y);
        matrix4x4f y_rot = linear_transforms::rotate3y(0.001f * delta_x);
        vector4f new_eye = x_rot * y_rot * eye;
        set_eye_position(new_eye.dehomo());
        set_eye_reference_point(vector3f{0,0,0});
        
    }
}

void freecam_scene::mouse_wheel_event(int delta) {
    eye_radius += 0.5f * delta;
}
