#include "demo.h"
#include "framebuffer.h"
#include "math_detail.h"
#include "math_util.h"
#include "vector.h"
#include "vector_util.h"
#include "SDL.h"
#include <cmath>

#include <iostream>
#include "util.h"

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
    set_eye_orientation(vector3f{0.0f, 1.0f, 0.0f});
    
    set_view_to_view_plane_distance(2);
    
    fern.render(*this, fb);
}

void demo::render_fern_3d(framebuffer &fb) {
    float t = clock.seconds();

    set_eye_position(vector3f{10 * cosf(0.3 * t), 4.0f, 10 * sinf(0.3f * t)});
    set_eye_reference_point(vector3f{0.0f, 2.0f, 0.0f});
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
    create_fern_recursive(vector3f{0.0f, 0.0f, 0.0f}, vector3f{0.0f, 3.0f, 0.0f},
                          color(1.0f, 1.0f, 1.0f, 1.0f), 4);
}

void demo::create_fern_recursive(const vector3f &root, const vector3f &tip,
                                 const color &root_color, int generations) {
    const float pi = detail::pi<float>();
    
    fern.add_line(root, tip, root_color, root_color);

    if (generations <= 1)
        return;

    vector3f stem(tip - root); // stem is a vector such that root + stem is the line drawn
    vector3f stem_direction(stem.unit());
    const float stem_length = stem.length();
    
    const int nodes = 2; // the points from which new lines branch
    const int branches_per_node = 5; // new branches per each such node

    for (int n = 0; n < nodes; ++n) {
        float node_pos_on_stem = (n + 1.0f) / (nodes + 1.0f);
        vector3f branch_root = root + node_pos_on_stem * stem;
        
        // create a line parallel to stem, p1->p2, that is guaranteed to != stem, then use
        // it to find the nearest point to the node and get a unit vector in its direction
        vector3f offset_point(branch_root + vector3f{stem_direction.y(), stem_direction.z(), stem_direction.x()});
        vector3f perpendicular_point = closest_point_on_line(branch_root, offset_point, stem_direction);
        vector3f stem_unit_normal((perpendicular_point - branch_root).unit());
        
        for (int b = 0; b < branches_per_node; ++b) {
            float angle = 2.0f * pi * b / branches_per_node;
            vector3f branch_normal_direction = around_axis(stem_unit_normal, stem, angle);
            vector3f branch_direction = (branch_normal_direction + stem_direction).unit();
            
            create_fern_recursive(branch_root, branch_root + 0.8f * (1.0f - node_pos_on_stem) * stem_length * branch_direction,
                                  root_color, generations - 1);
        }
    }
}
