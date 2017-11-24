#include "demo.h"
#include "framebuffer.h"
#include "math_detail.h"
#include "math_util.h"
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

    set_eye_position(vector3f{10 * cosf(0.3 * t), 4.0f, 10 * sinf(0.3f * t)});
    set_eye_reference_point(vector3f{0.0f, 2.0f, 0.0f});
    set_eye_orientation(vector3f{0.0f, 2.0f, 0.0f});
    
    set_view_to_view_plane_distance(2);
    
    fern.render(*this, fb);
}

void demo::render_fern_3d(framebuffer &fb) {
    float t = clock.seconds();

    set_eye_position(vector3f{10 * cosf(0.3 * t), 4.0f, 10 * sinf(0.3f * t)});
    set_eye_reference_point(vector3f{0.0f, 2.0f, 0.0f});
    set_eye_orientation(vector3f{0.0f, 2.0f, 0.0f});
    
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
    create_fern_recursive(vector3f{0.0f, 0.0f, 0.0f}, vector3f{0.0f, 3.0f, 0.0f},
                          color(1.0f, 1.0f, 1.0f, 1.0f), 2);
}

#include <iostream>
#include "util.h"

void demo::create_fern_recursive(const vector3f &root, const vector3f &tip,
                                 const color &root_color, int generations) {
    const float pi = detail::pi<float>();

    vector3f rotation_axis = (tip - root).unit();
    vector3f direction{1, 0, 0};

    for (int b = 0; b < 5; ++b) {
        float angle = 2.0f * pi * b / 5.0f;
        vector3f vec = cosf(angle) * direction + sinf(angle) * (direction.cross(rotation_axis)) + (1 - cos(angle)) * direction.dot(rotation_axis) * direction;
    }



    
    /*
    




    
    
    vector3f top_direction = len *
        vector3f{cosf(phi) * cosf(theta),
                 sinf(theta),
                -sinf(phi) * cosf(theta)};
    color top_color(0.0f, 0.2f, 0.1f, 1.0f);

    fern.add_line(root, root + top_direction, root_color, top_color);

    if (generations > 0) {
        for (int j = 0; j < theta_branches; ++j) {
            float branch_multiplier = 1.0f - powf(0.5f, j + 1);

            for (int i = 0; i < phi_branches; ++i) {
                create_fern_recursive(root + branch_multiplier * top_direction,
                                      color(root_color, top_color, branch_multiplier),
                                      (1.0f - branch_multiplier) * len,
                                      2.0f * pi * i / phi_branches,
                                      theta + 1.55f,
                                      generations - 1);
            }
        }
        }*/
}
