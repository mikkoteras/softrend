#include "scene.h"
#include "ambient_light.h"
#include "directional_light.h"
#include "framebuffer.h"
#include "material_library.h"
#include "viewport_transforms.h"

using namespace math;
using namespace std;

scene::scene() :
    eye_position{0, 0, 1},
    eye_direction{0, 0, -1},
    eye_up{0, 1, 0},
    viewing_distance(1),
    world_to_view_matrix(matrix4x4f::identity()),
    world_to_view_matrix_dirty(true) {
}

scene::~scene() {
}

void scene::start() {
    clock.start();
}

void scene::key_down_event(int, bool) {
}

void scene::mouse_move_event(int, int, bool) {
}

void scene::mouse_wheel_event(int) {
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

color scene::shade(const vector3f &surface_normal) const {
    return lights.sum(surface_normal);
}

const math::vector3f &scene::get_eye_position() const {
    return eye_position;
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

void scene::add_ambient_light(const color &light_color) {
    lights.add(move(make_unique<ambient_light>(light_color)));
}

void scene::add_directional_light(const vector3f &direction, const color &light_color) {
    lights.add(move(make_unique<directional_light>(direction, light_color)));
}

void scene::add_light(unique_ptr<light> l) {
    lights.add(move(l));
}
