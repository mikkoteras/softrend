#include "demo.h"
#include "framebuffer.h"
#include "vector.h"
#include "SDL.h"
#include <cmath>

using namespace math;
using namespace std;

demo::demo() {
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
    
    set_eye_position(vector3f{10 * cosf(t), 2.0f, 10 * sinf(t)});
    set_eye_reference_point(vector3f{0.0f, 1.0f, 0.0f});
    set_eye_orientation(vector3f{0.0f, 1.0f, 0.0f});
    
    set_view_to_view_plane_distance(2);
    
    fern.render(*this, fb);
}

void demo::render_fern_3d(framebuffer &fb) {
    float t = clock.seconds();
    
    set_eye_position(vector3f{10 * cosf(t), 2.0f, 10 * sinf(t)});
    set_eye_reference_point(vector3f{0.0f, 1.0f, 0.0f});
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
                          3.0f,
                          0.0f,
                          3.14159265f / 2.0f,
                          4);
}

void demo::create_fern_recursive(const vector3f &root,
                                 const color &root_color,
                                 float len, float phi, float theta, int generations) {
    vector3f top_direction = len * vector3f{
        cosf(phi) * cosf(theta),
        sinf(theta),
        -sinf(phi) * cosf(theta)
    };
    color top_color(0.0f, 0.2f, 0.1f, 1.0f);
    
    fern.add_line(root, root + top_direction, root_color, top_color);

    const int branches = 7;
    float branch_position[branches]               = {  0.2f,  0.4f,   0.55f, 0.65f,  0.75f,  0.90f,  1.0f  };
    float branch_phi_direction_offset[branches]   = {  1.1f,  2.85f,  4.5f,  1.8f,   5.75f, -2.65f, -0.6f  };
    float branch_theta_direction_offset[branches] = { -0.49f, 0.47f, -0.46f, 0.44f, -0.33f,  0.39f,  0.22f };
    float branch_length[branches]                 = {  0.6f,  0.5f,   0.4f,  0.3f,   0.2f,   0.1f,   0.1f  };

    if (generations > 0)
        for (int i = 0; i < branches; ++i)
            create_fern_recursive(root + branch_position[i] * top_direction,
                                  color(root_color, top_color, branch_position[i]),
                                  branch_length[i] * len,
                                  branch_phi_direction_offset[i] + phi,
                                  branch_theta_direction_offset[i] + theta,
                                  generations - 1);
}
