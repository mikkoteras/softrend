#include "freecam_scene.h"
#include "linear_transforms.h"
#include "math_detail.h"
#include "math_util.h"
#include "vector.h"
#include "SDL.h"

using namespace math;

freecam_scene::freecam_scene(float initial_eye_radius) :
    show_coords(false),
    coords(color(0.7f, 0.6f, 0.6f, 1.0f),
           color(0.6f, 0.7f, 0.6f, 1.0f),
           color(0.6f, 0.6f, 0.7f, 1.0f)),
    eye_azimuth_angle(0.0f),
    eye_polar_angle(0.0f),
    eye_radius(initial_eye_radius),
    eye_twist_angle(0.0f) {
}

freecam_scene::~freecam_scene() {
}

void freecam_scene::point_of_interest(const math::vector3f &p) {
    eye_reference_point = p;
}

void freecam_scene::render(framebuffer &fb) {
    // Normalize spherical coordinates
    const float pi = math::detail::pi<float>();
    float a = eye_azimuth_angle / (2.0f * pi);
    eye_azimuth_angle = 2.0f * pi * (a - floorf(a)); // 0 <= azimuth < 2*pi
    math::clamp<float>(eye_polar_angle, -pi / 2.0f + 0.0001f, pi / 2.0f - 0.0001f);  // -pi < polar < pi
    eye_radius = std::max(2.00001f, eye_radius); // viewer to plane distance < eye distance
    float b = eye_twist_angle / (2.0f * pi);
    eye_twist_angle = 2.0f * pi * (b - floorf(b)); // 0 <= twist < 2*pi

    float eye_x = eye_radius * sinf(eye_azimuth_angle) * cos(eye_polar_angle);
    float eye_y = eye_radius * sin(eye_polar_angle);
    float eye_z = eye_radius * cosf(eye_azimuth_angle) * cos(eye_polar_angle);

    set_eye_position(eye_reference_point + vector3f{eye_x, eye_y, eye_z});
    set_eye_reference_point(eye_reference_point);
    set_eye_orientation(vector3f{0.0f, cosf(eye_twist_angle), sinf(eye_twist_angle), 0.0f});

    set_view_to_view_plane_distance(2);

    if (show_coords)
        coords.render(*this, fb);
}

void freecam_scene::key_down_event(int sdl_keycode, bool ctrl_is_down) {
    if (ctrl_is_down) {
        if (sdl_keycode == SDLK_c)
            show_coords = !show_coords;
        else if (sdl_keycode == SDLK_r) { // reset scene
            eye_azimuth_angle = 0.0f;
            eye_polar_angle = 0.0f;
            eye_radius = 10.0f;
            eye_twist_angle = 0.0f;
        }
    }
    else {
        if (sdl_keycode == SDLK_w)
            eye_polar_angle += 0.1f;
        else if (sdl_keycode == SDLK_s)
            eye_polar_angle -= 0.1f;
        else if (sdl_keycode == SDLK_a)
            eye_azimuth_angle -= 0.1f;
        else if (sdl_keycode == SDLK_d)
            eye_azimuth_angle += 0.1f;
        else if (sdl_keycode == SDLK_r)
            eye_radius -= 0.5f;
        else if (sdl_keycode == SDLK_f)
            eye_radius += 0.5f;
        else if (sdl_keycode == SDLK_q)
            eye_twist_angle += 0.1f;
        else if (sdl_keycode == SDLK_e)
            eye_twist_angle -= 0.1f;
    }
}

void freecam_scene::mouse_move_event(int delta_x, int delta_y, bool left_button_is_down) {
    if (left_button_is_down) {
        eye_azimuth_angle -= 0.001f * delta_x;
        eye_polar_angle += 0.001f * delta_y;
    }
}

void freecam_scene::mouse_wheel_event(int delta_x, int delta_y) {
    eye_radius += 0.5f * delta_y;
    eye_twist_angle += 0.1f * delta_x;
}
