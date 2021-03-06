#include "freecam_scene.h"
#include "command_line.h"
#include "math/linear_transforms.h"
#include "math/math_detail.h"
#include "math/math_util.h"
#include "math/vector.h"
#include "SDL.h"
#include <algorithm>

using namespace math;
using namespace math::detail;

freecam_scene::freecam_scene(const command_line &cl, float initial_eye_radius) :
    scene(cl),
    eye_radius(initial_eye_radius),
    eye_twist_angle(0.0f),
    window_width(cl.width()),
    window_height(cl.height()) {

    const float pi = detail::pi<float>();
    eye_azimuth_angle = 1.5f * pi;
}

freecam_scene::~freecam_scene() {
}

void freecam_scene::set_point_of_interest(const math::vector3f &p) {
    point_of_interest = p;
}

void freecam_scene::compose() {
    // Normalize spherical coordinates
    const float pi = detail::pi<float>();

    eye_azimuth_angle = period<float>(eye_azimuth_angle, 0.0f, 2.0f * pi);
    clamp<float>(eye_polar_angle, -pi / 2.0f + 0.0001f, pi / 2.0f - 0.0001f); // -pi < polar < pi
    eye_radius = std::max(2.00001f, eye_radius); // viewer to plane distance < eye distance
    eye_twist_angle = period<float>(eye_twist_angle, 0.0f, 2.0f * pi);

    float eye_x = eye_radius * cos<float>(eye_azimuth_angle) * cos<float>(eye_polar_angle);
    float eye_y = eye_radius * sin<float>(eye_polar_angle);
    float eye_z = eye_radius * -sin<float>(eye_azimuth_angle) * cos<float>(eye_polar_angle);

    set_eye_position(point_of_interest + vector3f{eye_x, eye_y, eye_z});
    set_eye_reference_point(point_of_interest);
    set_eye_orientation(vector3f{0.0f, cos<float>(eye_twist_angle), sin<float>(eye_twist_angle)});
    set_fov(120.0f / (2.0f * math::detail::pi<float>()));
}

void freecam_scene::key_down_event(int sdl_keycode, bool ctrl_is_down) {
    const float pi = detail::pi<float>();

    if (ctrl_is_down) {
        if (sdl_keycode == SDLK_r) { // reset scene
            eye_azimuth_angle = 1.5f * pi;
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
        eye_azimuth_angle -= 12.0f * delta_x / window_width;
        eye_polar_angle += 5.0f * delta_y / window_height;
    }
}

void freecam_scene::mouse_wheel_event(int delta_x, int delta_y) {
    eye_radius += 0.5f * delta_y;
    eye_twist_angle += 0.1f * delta_x;
}
