#include "demo.h"
#include "framebuffer.h"
#include "math_detail.h"
#include "math_util.h"
#include "vector.h"
#include "vector_util.h"
#include "SDL.h"
#include <algorithm>
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
    case dot_curve:
        render_dot_curve(fb);
        break;
    case line_spiral:
        render_line_spiral(fb);
        break;
    case fern_still:
        render_fern_still(fb);
        break;
    case fern_3d:
        render_fern_3d(fb);
        break;
    }

    if (show_coord_sys)
        coord_sys.render(*this, fb);
}

void demo::render_dot_curve(framebuffer &fb) {
    float cx = fb.pixel_width() / 2.0f;
    float cy = fb.pixel_height() / 2.0f;

    for (float t = 10.0f; t < 200.0f ; t += 0.002f) {
        float x1 = cx + (t + t * cos(5 * t)) * cosf(t);
        float y1 = cy + (t + t * sin(3 * t)) * sinf(t);
        fb.set_pixel(x1, y1, color(fabs(sinf(t)),
                                   fabs(sin(t / 200.0f * detail::pi<float>())),
                                   fabs(sinf(t + detail::pi<float>() / 2.0f)),
                                   1.0f));
    }
}

void demo::render_line_spiral(framebuffer &fb) {
    float cx = fb.pixel_width() / 2.0f;
    float cy = fb.pixel_height() / 2.0f;

    for (float t = 10.0f; t < 500.0f; t += 1.5f) {
        float x1 = cx + t * cosf(0.1f * t);
        float y1 = cy + t * sinf(0.1f * t);
        float x2 = cx + 1.2f * t * cosf(0.1f * t + detail::pi<float>() / 4.0f);
        float y2 = cy + 1.2f * t * sinf(0.1f * t + detail::pi<float>() / 4.0f);
        line::render(fb, x1, y1, 0, color(1, t / 500.0f, 0, 1), x2, y2, 0, color(0, t / 500.f, 1, 1));
    }
}

void demo::render_fern_still(framebuffer &fb) {
    set_eye_position(vector3f{3, 4, 10});
    set_eye_reference_point(vector3f{0.0f, 4.0f, 0.0f});
    set_eye_orientation(vector3f{0.0f, 1.0f, 0.0f});
    
    set_view_to_view_plane_distance(2.0f);

    fern.render(*this, fb);
}

void demo::render_fern_3d(framebuffer &fb) {
    float t = clock.seconds();

    float eye_x = 3.0f * cosf(0.4 * t);
    float eye_y = 4.0f + 3.0f * sinf(0.5f * t);
    float eye_z = 10.0f + sinf(0.6f * t);

    set_eye_position(vector3f{eye_x, eye_y, eye_z});
    set_eye_reference_point(vector3f{0.0f, 4.0f, 0.0f});
    set_eye_orientation(vector3f{0.0f, 1.0f, 0.0f});
    
    set_view_to_view_plane_distance(2.0f);

    fern.render(*this, fb);
}

void demo::key_down_event(int sdl_keycode, bool ctrl_is_down) {
    if (ctrl_is_down) {
        if (sdl_keycode == SDLK_c)
            show_coord_sys = !show_coord_sys;
    }
    else {
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
}

void demo::create_fern() {
    create_fern_recursive(vector3f{0.0f, 0.0f, 0.0f}, vector3f{0.0f, 5.5f, 0.0f}, 5);
}

void demo::create_fern_recursive(const vector3f &root, const vector3f &tip, int generations) {
    const float pi = detail::pi<float>();
    color c;

    if (generations >= 4)
        c = color(.61f, 0.47f, .29f, 1);
    else if (generations == 3)
        c = color(.25f, .50f, .0f, 1);
    else if (generations == 2)
        c = color(.06f, .89f, .04f, 1);
    else
        c = color(.37f, .89f, .50f, 1);

    color c1 = (0.2f * root.z() + 1.0f) * 0.7f * c;
    color c2 = (0.2f * tip.z() + 1.0f) * 1.3f * c;
    c1.clamp();
    c2.clamp();
    
    fern.add_line(root, tip, c1, c2);

    if (generations <= 1)
        return;

    vector3f stem(tip - root); // stem is a vector such that root + stem is the line drawn
    vector3f stem_direction(stem.unit());
    const float stem_length = stem.length();
    
    const int nodes = 3; // the points from which new lines branch
    const int branches_per_node = 4; // new branches per each such node

    for (int n = 0; n < nodes; ++n) {
        float node_pos_on_stem = (n + 1.0f) / nodes;
        float branch_length_from_node =  0.7f * stem_length * (0.3f + 0.7 * (1.0f - node_pos_on_stem));
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
            
            create_fern_recursive(branch_root, branch_root + branch_length_from_node * branch_direction, generations - 1);
        }
    }
}
