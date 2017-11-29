#include "scene.h"
#include "directional_light.h"
#include "framebuffer.h"
#include "material_library.h"
#include "viewport_transforms.h"
#include <limits>

using namespace math;
using namespace std;

scene::scene() :
    eye_position{0, 0, 1},
    eye_direction{0, 0, -1},
    eye_up{0, 1, 0},
    viewing_distance(1),
    world_to_view_matrix(matrix4x4f::identity()),
    world_to_view_matrix_dirty(true),
    framebuffer_visible_volume(vector3f{0.0f, 0.0f, 0.0f}),
    stop_requested(false) {
}

scene::~scene() {
}

void scene::prerender(const framebuffer &fb) {
    framebuffer_visible_volume = bounding_box(vector3f{0.0f, 0.0f, 0.0f});
    framebuffer_visible_volume.stretch(vector3f{static_cast<float>(fb.pixel_width()),
                                                static_cast<float>(fb.pixel_height()),
                                                -std::numeric_limits<float>::infinity()});    
}

void scene::key_down_event(int, bool) {
}

void scene::mouse_move_event(int, int, bool) {
}

void scene::mouse_wheel_event(int, int) {
}

scene_info &scene::get_scene_info() {
    return info;
}

matrix4x4f scene::world_to_view() {
    using namespace math::viewport_transforms;

    if (world_to_view_matrix_dirty) {
        world_to_view_matrix =
            simple_perspective_projection<float>(viewing_distance) *
            world_to_view_plane<float>(eye_position, eye_direction, eye_up);
        world_to_view_matrix_dirty = false;
    }

    return world_to_view_matrix;
}

material_library &scene::materials() {
    return mat_lib;
}

const bounding_box &scene::visible_volume() const {
    return framebuffer_visible_volume;
}

const light_list &scene::light_sources() const {
    return lights;
}

light_list &scene::light_sources() {
    return lights;
}

const math::vector3f &scene::get_eye_position() const {
    return eye_position;
}

void scene::start() {
    clock.start();
}

void scene::stop() {
    stop_requested = true;
}

bool scene::stopped() const {
    return stop_requested;
}

void scene::set_eye_position(const vector3f &position) {
    eye_position = position;
    world_to_view_matrix_dirty = true;
}

void scene::set_eye_direction(const vector3f &direction) {
    eye_direction = direction;
    world_to_view_matrix_dirty = true;
}

void scene::set_eye_reference_point(const vector3f &look_at_point) {
    eye_direction = look_at_point - eye_position;
    world_to_view_matrix_dirty = true;
}

void scene::set_eye_orientation(const vector3f &up_direction) {
    eye_up = up_direction;
    world_to_view_matrix_dirty = true;
}

void scene::set_view_to_view_plane_distance(float d) {
    viewing_distance = d;
    world_to_view_matrix_dirty = true;
}
