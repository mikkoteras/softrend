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
    fov(120.0f / (2.0f * detail::pi<float>())),
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

void scene::postrender() {
    get_scene_info().update(*this);
}

void scene::cycle_shading_model() {
    int rm = (shading_model + 1) % (max_shading_model + 1);
    shading_model = static_cast<shading_model_t>(rm);
}

void scene::set_shading_model(shading_model_t rm) {
    shading_model = rm;
}

shading_model_t scene::get_shading_model() const {
    return shading_model;
}

void scene::set_normal_visualization(bool setting) {
    visualize_normals = setting;
}

bool scene::get_normal_visualization() const {
    return visualize_normals;
}

void scene::set_reflection_vector_visualization(bool setting) {
    visualize_reflection_vectors = setting;
}

bool scene::get_reflection_vector_visualization() const {
    return visualize_reflection_vectors;
}

void scene::set_wireframe_visualization(bool setting) {
    visualize_wireframe = setting;
}

bool scene::get_wireframe_visualization() const {
    return visualize_wireframe;
}

double scene::get_animation_time() const {
    return clock.seconds();
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

benchmark &scene::get_benchmark() {
    return mark;
}

matrix4x4f scene::world_to_view() {
    using namespace math::viewport_transforms;

    if (world_to_view_matrix_dirty) {
        world_to_view_matrix =
            normalizing_perspective_projection<float>(fov) *
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

void scene::set_fov(float fov_radians) {
    fov = fov_radians;
    world_to_view_matrix_dirty = true;
}
