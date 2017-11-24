#include "demo.h"
#include "framebuffer.h"
#include "math_detail.h"
#include "vector.h"
#include "SDL.h"
#include <cmath>

using namespace math;
using namespace std;

demo::demo() :
    scene() {
    
    create_fern();
}

demo::~demo() {
}

void demo::render(framebuffer &fb) {
    switch (stage) {
    case fern_still:
        render_fern_still(fb);
        break;
    case fern_3d:
        render_fern_3d(fb);
        break;
    }
}

void demo::render_fern_still(framebuffer &fb) {
    float t = 0.0f;
    
    set_eye_position(vector3f{10 * cosf(t), 4.0f, 10 * sinf(t)});
    set_eye_reference_point(vector3f{0.0f, 1.0f, 0.0f});
    set_eye_orientation(vector3f{0.0f, 2.0f, 0.0f});
    
    set_view_to_view_plane_distance(2);
    
    fern.render(*this, fb);
}

void demo::render_fern_3d(framebuffer &fb) {
    float t = clock.seconds();
    
    set_eye_position(vector3f{10 * cosf(t), 85.0f, 10 * sinf(t)});
    set_eye_reference_point(vector3f{0.0f, 0.0f, 0.0f});
    set_eye_orientation(vector3f{0.0f, 1.0f, 0.0f});
    
    set_view_to_view_plane_distance(2);
    
    fern.render(*this, fb);
}

void demo::key_down_event(int sdl_keycode, bool ctrl_is_down) {
    if (sdl_keycode == SDLK_PAGEUP) {
        if (stage > min_stage) {
            stage = (stage_t)((int)stage - 1);
            clock.reset();
        }
    }
    else if (sdl_keycode == SDLK_PAGEDOWN) {
        if (stage < max_stage) {
            stage = (stage_t)((int)stage + 1);
            clock.reset();
        }
    }
}

void demo::create_fern() {
    create_fern_recursive(vector3f{0.0f, 0.0f, 0.0f},
                          color(0.0f, 1.0f, 0.5f, 1.0f),
                          5.0f,
                          0.0f,
                          detail::pi<float>() / 2.0f,
                          3);
}

void demo::create_fern_recursive(const vector3f &root,
                                 const color &root_color,
                                 float len, float phi, float theta, int generations) {
    vector3f top_direction = len * vector3f{cosf(phi) * cosf(theta), sinf(theta), -sinf(phi) * cosf(theta)};
    color top_color(0.0f, 0.2f, 0.1f, 1.0f);

    fern.add_line(root, root + top_direction, root_color, top_color);

    if (generations > 0) {
        const int phi_branches = 5;
        const int theta_branches = 7;

        for (int j = 0; j < theta_branches; ++j) {
            float branch_pos = 1.0f - powf(2.0f / 3.0f, j + 1);

            for (int i = 0; i < phi_branches; ++i) {
                create_fern_recursive(root + branch_pos * top_direction,
                                      color(root_color, top_color, branch_pos),
                                      len / (3 * j + 2),
                                      2.0f * detail::pi<float>() * i / phi_branches,
                                      theta + 0.7f,
                                      generations - 1);
            }
        }
    }
}
