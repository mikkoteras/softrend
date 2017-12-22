#include "scene.h"
#include "directional_light.h"
#include "framebuffer.h"
#include "linear_transforms.h"
#include "material_library.h"
#include "viewport_transforms.h"
#include <algorithm>
#include <limits>

using namespace math;
using namespace std;

scene::scene() :
    eye_position{0, 0, 1},
    eye_direction{0, 0, -1},
    eye_up{0, 1, 0},
    fov(120.0f / (2.0f * detail::pi<float>())),
    world_to_view_matrix(matrix4x4f::identity()),
    framebuffer_visible_volume(vector3f{0.0f, 0.0f, 0.0f}),
    coords(this, color(0.7f, 0.6f, 0.6f, 1.0f), color(0.6f, 0.7f, 0.6f, 1.0f), color(0.6f, 0.6f, 0.7f, 1.0f)),
    stop_requested(false) {
}

scene::~scene() {
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

void scene::set_coordinate_system(bool setting) {
    show_coordinate_system = setting;
}

bool scene::get_coordinate_system() const {
    return show_coordinate_system;
}

double scene::get_animation_time() const {
    return clock.seconds();
}

void scene::render(framebuffer &fb) {
    compose();
    construct_world_to_view(fb);
    compute_visible_volume(fb);
    prerender(fb);
    
    for (mesh *m: meshes) // TODO this could basically be threaded (but how much does it help?)
        transform_coordinates(*m);

    sort_triangles();

    for (const triangle_distance &t: triangle_order)
        triangles[t.triangle_index].render(fb, *this);

    for (const line &l: lines)
        l.render(fb, *this);

    overlay_wireframe_visualization(fb);
    overlay_normal_visualization(fb);
    overlay_reflection_vector_visualization(fb);
    postrender(fb);
    info.update(*this);
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

int scene::add_vertex(const math::vector3f &v) {
    local_coordinates.push_back(vector4f(v, 1.0f));
    world_coordinates.push_back(vector3f());
    view_coordinates.push_back(vector3f());
    return static_cast<int>(local_coordinates.size() - 1);
}
    
int scene::add_vertex_normal(const math::vector3f &vn) {
    local_normals.push_back(vector4f(vn, 0.0f));
    world_normals.push_back(vector3f());
    return static_cast<int>(local_normals.size() - 1);
}

void scene::add_triangle(int vi1, int vi2, int vi3,
                  int ni1, int ni2, int ni3,
                  const math::vector3f &uv1, const math::vector3f &uv2, const math::vector3f &uv3,
                  const material *mat) {
    triangles.push_back(triangle(vi1, vi2, vi3, ni1, ni2, ni3, uv1, uv2, uv3, mat));
}

void scene::add_triangle(int vi1, int vi2, int vi3, int ni1, int ni2, int ni3, const material *mat) {
    triangles.push_back(triangle(vi1, vi2, vi3, ni1, ni2, ni3, mat));

}

void scene::add_line(int v1, int v2, const color &c1, const color &c2) {
    lines.push_back(line(v1, v2, c1, c2));
}

const matrix4x4f &scene::world_to_view() {
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

const vector4f *scene::local_coordinate_data() const {
    return local_coordinates.data();
}

const vector3f *scene::world_coordinate_data() const {
    return world_coordinates.data();
}

const vector3f *scene::world_normal_data() const {
    return world_normals.data();
}

const vector3f *scene::view_coordinate_data() const {
    return view_coordinates.data();
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

void scene::add_mesh(mesh *caller) {
    meshes.push_back(caller);
}

void scene::prerender(framebuffer&) {
}

void scene::postrender(framebuffer&) {
}

void scene::set_eye_position(const vector3f &position) {
    eye_position = position;
}

void scene::set_eye_direction(const vector3f &direction) {
    eye_direction = direction;
    eye_direction.normalize();
}

void scene::set_eye_reference_point(const vector3f &look_at_point) {
    eye_direction = look_at_point - eye_position;
    eye_direction.normalize();
}

void scene::set_eye_orientation(const vector3f &up_direction) {
    eye_up = up_direction;
    eye_up.normalize();
}

void scene::set_fov(float fov_radians) {
    fov = fov_radians;
}

void scene::construct_world_to_view(const framebuffer &fb) {
    using namespace math::linear_transforms;
    using namespace math::viewport_transforms;

    float min_axis = min(fb.pixel_width(), fb.pixel_height());
    
    matrix4x4f view_position = world_to_view_plane<float>(eye_position, eye_direction, eye_up);
    matrix4x4f projection = normalizing_perspective_projection<float>(fov);
    matrix4x4f scale_to_screen_coords = scale3<float>(min_axis / 2.0f);
    matrix4x4f translate_to_screen_coords = translate3<float>(fb.pixel_width() / 2.0f, fb.pixel_height() / 2.0f, 0.0f);

    world_to_view_matrix = translate_to_screen_coords * scale_to_screen_coords * projection * view_position;
}

void scene::compute_visible_volume(const framebuffer &fb) {
    framebuffer_visible_volume = bounding_box(vector3f{0.0f, 0.0f, 0.0f});
    framebuffer_visible_volume.stretch(vector3f{static_cast<float>(fb.pixel_width()),
                                                static_cast<float>(fb.pixel_height()),
                                                -std::numeric_limits<float>::infinity()});    
}

void scene::transform_coordinates(mesh &of_mesh) {
    const matrix4x4f &local_to_world = of_mesh.local_to_world();

    int min = of_mesh.min_normal_index();
    int max = of_mesh.max_normal_index();

    for (int i = min; i <= max; ++i)
        world_normals[i] = (local_to_world * local_normals[i]).dehomo();

    min = of_mesh.min_coordinate_index();
    max = of_mesh.max_coordinate_index();

    for (int i = min; i <= max; ++i)
        world_coordinates[i] = (local_to_world * local_coordinates[i]).dehomo();
    
    const matrix4x4f local_to_view = world_to_view_matrix * local_to_world;

    for (int i = min; i <= max; ++i)
        view_coordinates[i] = (local_to_view * local_coordinates[i]).dehomo_with_divide();
}

void scene::sort_triangles() {
    // TODO: opaque v. translucent polys in different lists, reverse painter's etc
    triangle_order.resize(triangles.size());

    for (unsigned i = 0; i < triangles.size(); ++i) {
        triangle_order[i].triangle_index = i;
        const int *vertex_indices = triangles[i].vertex_indices();
        float z = view_coordinates[vertex_indices[0]].z();
        z = std::min(z, view_coordinates[vertex_indices[1]].z());
        z = std::min(z, view_coordinates[vertex_indices[2]].z());
        triangle_order[i].z_coordinate = z;
    }

    std::sort(triangle_order.begin(), triangle_order.end());
}

void scene::overlay_wireframe_visualization(framebuffer &fb) {
    // TODO: remove duplicates
    if (visualize_wireframe)
        for (triangle t: triangles) {
            const int *vertex_indices = t.vertex_indices();
            
            for (int i = 0; i < 3; ++i) {
                const vector3f &v1 = view_coordinates[vertex_indices[i]];
                const vector3f &v2 = view_coordinates[vertex_indices[(i + 1) % 3]];
                line::render(fb,
                             v1.x(), v1.y(), v1.z() + 0.01f, color(0.0f, 0.0f, 0.0f, 1.0f),
                             v2.x(), v2.y(), v2.z() + 0.01f, color(0.0f, 0.0f, 0.0f, 1.0f));
            }
        }
}

void scene::overlay_normal_visualization(framebuffer &fb) {
    if (visualize_normals)
        for (const triangle &t: triangles)
            t.visualize_normals(fb, *this);
}

void scene::overlay_reflection_vector_visualization(framebuffer &fb) {
    if (visualize_reflection_vectors) {
        size_t step = triangles.size() / 250;
        step = max<size_t>(step, 1);

        for (size_t i = 0; i < triangles.size(); i += step)
            triangles[i].visualize_reflection_vectors(fb, *this);
    }
}
