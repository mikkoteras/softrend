#include "demo.h"
#include "color3.h"
#include "command_line.h"
#include "framebuffer.h"
#include "importer.h"
#include "point_light.h"
#include "math/math_detail.h"
#include "math/math_util.h"
#include "math/vector.h"
#include "math/vector_util.h"
#include "SDL.h"
#include <algorithm>

using namespace math;
using namespace math::detail;
using namespace std;

demo::demo(const command_line &cl) :
    scene(cl),
    fern(this),
    slab(this) {

    create_fern();

    importer::load_wavefront_object(slab, "slab.obj", materials(), cl.verbose());
    slab.set_scaling(1.0f / slab.local_bounding_box().max_semiaxis());
}

demo::~demo() {
}

void demo::compose() {
    hide_all_meshes();

    switch (stage) {
    case fern_still:
    case fern_3d:
        compose_fern();
        break;
    case slab_demo:
        compose_slab_demo();
        break;
    default:
        break;
    }
}

void demo::prerender(const scene_render_context &scene_context) {
    switch (stage) {
    case dot_curve:
        render_dot_curve(scene_context);
        break;
    case line_spiral:
        render_line_spiral(scene_context);
        break;
    default:
        break;
    }
}

void demo::render_dot_curve(const scene_render_context &scene_context) {
    float cx = scene_context.target->pixel_width() / 2.0f;
    float cy = scene_context.target->pixel_height() / 2.0f;

    float t = clock.seconds() / 10.0f;

    for (float u = 0.0f; u < 200.0f ; u += 0.08f) {
        float v = t + u;
        float x1 = cx + (v + v * cos<float>(5.0f * v)) * cos<float>(v);
        float y1 = cy + (v + v * sin<float>(3.0f * v)) * sin<float>(v);
        scene_context.target->set_pixel_with_xy_clip(x1, y1, color3(fabs(sin<float>(v)),
                                                                    fabs(sin<float>(v / 200.0f * pi<float>())),
                                                                    fabs(sin<float>(v + pi<float>() / 2.0f))));
    }
}

void demo::render_line_spiral(const scene_render_context &scene_context) {
    float cx = scene_context.target->pixel_width() / 2.0f;
    float cy = scene_context.target->pixel_height() / 2.0f;

    float t = clock.seconds();
    
    for (float s = 10.0f; s < 1000.0f; s += 0.5f) {
        float x0 = cx + s * cos<float>(0.1f * s);
        float y0 = cy + s * sin<float>(0.1f * s);

        float dx = 0.05f * s * cos<float>(s + t);
        float dy = 0.05f * s * sin<float>(s + t);
        
        float x1 = x0 - dx, x2 = x0 + dx;
        float y1 = y0 - dy, y2 = y0 + dy;

        color3 c1(abs<float>(cos<float>(t)), 0.0f, abs<float>(sin<float>(t + s)));
        color3 c2(abs<float>(sin<float>(s)), 0.0f, abs<float>(cos<float>(t + s)));

        line::render(*scene_context.target, x1, y1, 0, c1, x2, y2, 0, c2);
    }
}

void demo::compose_fern() {
    float t = 0.0;

    if (stage == fern_3d)
        t = clock.seconds();
    
    fern.set_visibility(true);

    fern.set_position(0.0f, 0.0f, 0.0f);
    fern.set_rotation(0.0f, 0.0f, 0.0f);

    float eye_x = 5.0f * (cos<float>(0.1f * sin(t) + pi<float>() / 2.0f));
    float eye_y = 3.0f + 0.2f * sin(0.5f * t);
    float eye_z = 5.0f * (sin<float>(0.1f * sin(t) + pi<float>() / 2.0f));

    set_eye_position(vector3f{eye_x, eye_y, eye_z});
    set_eye_reference_point(vector3f{0.0f, eye_y, 0.0f});
    set_eye_orientation(vector3f{0.0f, 1.0f, 0.0f});

    set_fov((90.0f / 360.0) * (2.0f * math::detail::pi<float>()));
}

void demo::compose_slab_demo() {
    float t = clock.seconds();
    slab.set_visibility(true);

    set_eye_position(vector3f{0.0f, 0.0f, 3.0f});
    set_eye_reference_point(vector3f{0.0f, 0.0f, 0.0f});
    set_eye_orientation(vector3f{0.0f, 1.0f, 0.0f});

    set_fov((90.0f / 360.0) * (2.0f * math::detail::pi<float>()));

    slab.set_position(0.0f, 0.0f, 0.0f);
    slab.set_rotation(0.8f * sin<float>(0.6f * t) + 0.8f, 0.0f, 0.1f);

    light_sources().clear();
    light_sources().set_ambient_light(0x505050_rgb);
    light_sources().add_light(point_light(vector3f{1.0f, 9.0f, 1.0f}, 0xFF0000_rgb));
    light_sources().add_light(point_light(vector3f{1.0f, 9.0f, 6.0f}, 0x00FF00_rgb));
    light_sources().add_light(point_light(vector3f{1.0f, 9.0f, 11.0f}, 0x0000FF_rgb));
}

void demo::key_down_event(int sdl_keycode, bool ctrl_is_down) {
    if (ctrl_is_down)
        ;
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
    color3 c;

    if (generations >= 4)
        c = 0x9B7749_rgb;
    else if (generations == 3)
        c = 0x3F7F00_rgb;
    else if (generations == 2)
        c = 0x0FE20A_rgb;
    else
        c = 0x5EE27F_rgb;

    color3 c1 = (0.2f * root.z() + 1.0f) * 0.7f * c;
    color3 c2 = (0.2f * tip.z() + 1.0f) * 1.3f * c;
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
        float branch_length_from_node =  0.7f * stem_length * (0.3f + 0.7f * (1.0f - node_pos_on_stem));
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

void demo::hide_all_meshes() {
    fern.set_visibility(false);
    slab.set_visibility(false);
}
