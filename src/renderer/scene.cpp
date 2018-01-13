#include "scene.h"
#include "command_line.h"
#include "directional_light.h"
#include "framebuffer.h"
#include "material_library.h"
#include "math/linear_transforms.h"
#include "math/viewport_transforms.h"
#include <algorithm>
#include <limits>
#include <thread>
#include <vector>

using namespace math;
using namespace std;

scene::scene(const command_line &cl) :
    fov(120.0f / (2.0f * detail::pi<float>())),
    framebuffer_visible_volume(vector3f{0.0f, 0.0f, 0.0f}),
    coords(this, 0xB29999_rgb, 0x99B299_rgb, 0x9999B2_rgb),
    num_threads(cl.rasterizer_threads()),
    threads(this, num_threads) {
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
    coords.get_mesh().set_visibility(setting);
}

bool scene::get_coordinate_system() const {
    return coords.get_mesh().get_visibility();
}

double scene::get_animation_time() const {
    return clock.seconds();
}

void scene::render(framebuffer &fb) {
    compose();

    render_context.target = &fb;
    render_context.parent_scene = this;
    render_context.eye = get_eye_position();
    render_context.light_sources = &lights;
    render_context.num_threads = num_threads;

    construct_world_to_view(fb);
    compute_visible_volume(fb);
    prerender(render_context);
    transform_coordinates();
    render_lines(fb);
    render_triangles();
    overlay_wireframe_visualization();
    overlay_normal_visualization();
    overlay_reflection_vector_visualization();
    postrender(render_context);
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

int scene::add_line(int v1, int v2, const color3 &c1, const color3 &c2) {
    lines.push_back(line(v1, v2, c1, c2));
    return static_cast<int>(lines.size() - 1);
}

int scene::add_triangle(int vi1, int vi2, int vi3,
                        int ni1, int ni2, int ni3,
                        const math::vector2f &uv1, const math::vector2f &uv2, const math::vector2f &uv3,
                        const material *mat) {
    triangles.push_back(triangle(vi1, vi2, vi3, ni1, ni2, ni3, uv1, uv2, uv3, mat));
    return static_cast<int>(triangles.size() - 1);
}

int scene::add_triangle(int vi1, int vi2, int vi3, int ni1, int ni2, int ni3, const material *mat) {
    triangles.push_back(triangle(vi1, vi2, vi3, ni1, ni2, ni3, mat));
    return static_cast<int>(triangles.size() - 1);
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

int scene::get_vertex_count() const {
    return static_cast<int>(local_coordinates.size());
}

int scene::get_normal_count() const {
    return static_cast<int>(local_normals.size());
}

int scene::get_visible_line_count() const {
    return num_visible_lines;
}

int scene::get_visible_triangle_count() const {
    return num_visible_triangles;
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

int scene::add_mesh(mesh *caller) {
    meshes.push_back(caller);
    return static_cast<int>(meshes.size() - 1);
}

void scene::prerender(const scene_render_context&) {
}

void scene::postrender(const scene_render_context&) {
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

void scene::transform_coordinates() {
    for (mesh *m: meshes) {
        const matrix4x4f &local_to_world = m->local_to_world();

        int min = m->min_normal_index();
        int max = m->max_normal_index();

        for (int i = min; i <= max; ++i) {
            world_normals[i] = (local_to_world * local_normals[i]).dehomo();
            world_normals[i].normalize();
        }

        min = m->min_vertex_index();
        max = m->max_vertex_index();

        for (int i = min; i <= max; ++i)
            world_coordinates[i] = (local_to_world * local_coordinates[i]).dehomo();
    
        const matrix4x4f local_to_view = world_to_view_matrix * local_to_world;

        for (int i = min; i <= max; ++i)
            view_coordinates[i] = (local_to_view * local_coordinates[i]).dehomo_with_divide();
    }
}

void scene::render_lines(framebuffer &fb) {
    num_visible_lines = 0;

    for (mesh *m: meshes) {
        int min = m->min_line_index();
        int max = m->max_line_index();

        for (int i = min; i <= max; ++i)
            lines[i].render(fb, *this);

        num_visible_lines += (max - min + 1);
    }
}

void scene::render_triangles() {
    triangle_order.resize(triangles.size()); // TODO maybe put this elsewhere
    int triangle_count = 0;

    // create a list of visible triangle indices and their farthest vertex z-coords,
    // the use it to sort the triangles triangles furthest to nearest from the screen
    for (mesh *m: meshes) {
        int min = m->min_triangle_index();
        int max = m->max_triangle_index();

        for (int i = min; i <= max; ++i) {
            const int *vertex_indices = triangles[i].vertex_indices();
            float farthest_vertex_z = view_coordinates[vertex_indices[0]].z();
            farthest_vertex_z = std::min(farthest_vertex_z, view_coordinates[vertex_indices[1]].z());
            farthest_vertex_z = std::min(farthest_vertex_z, view_coordinates[vertex_indices[2]].z());
            triangle_order[triangle_count].triangle_index = triangle_count;
            triangle_order[triangle_count++].z_coordinate = farthest_vertex_z;
        }
    }

    num_visible_triangles = triangle_count;

    auto first = triangle_order.begin();
    sort(first, first + triangle_count);

    // prepare triangle render contexts
    for (int i = 0; i < triangle_count; ++i) // TODO multi-thread
        triangles[triangle_order[i].triangle_index].prepare_for_render(render_context);

    // render multi-threaded
    threads.execute(&scene::render_triangles_threaded);
}

void scene::prepare_triangles_for_render_threaded(size_t thread_index) {
    for (size_t i = thread_index; i < num_visible_triangles; i += num_threads)
        triangles[triangle_order[i].triangle_index].prepare_for_render(render_context);
}

void scene::render_triangles_threaded(size_t thread_index) {
    // render opaque triangles using backward painter's algorithm
    for (int i = num_visible_triangles - 1; i >= 0; --i) {
        triangle &t = triangles[triangle_order[i].triangle_index];

        if (!t.has_transparency())
            t.render(render_context, thread_index);
    }

    // superimpose translucent triangles using forward painter's algorithm
    for (int i = 0; i < num_visible_triangles; ++i) {
        triangle &t = triangles[triangle_order[i].triangle_index];

        if (t.has_transparency())
            t.render(render_context, thread_index);
    }
}

void scene::overlay_wireframe_visualization() {
    // TODO: remove duplicates
    if (visualize_wireframe)
        for (triangle t: triangles) {
            const int *vertex_indices = t.vertex_indices();
            
            for (int i = 0; i < 3; ++i) {
                const vector3f &v1 = view_coordinates[vertex_indices[i]];
                const vector3f &v2 = view_coordinates[vertex_indices[(i + 1) % 3]];
                line::render(*render_context.target,
                             v1.x(), v1.y(), v1.z() + 0.01f, 0x191919_rgb,
                             v2.x(), v2.y(), v2.z() + 0.01f, 0x191919_rgb);
            }
        }
}

void scene::overlay_normal_visualization() {
    if (visualize_normals)
        for (triangle &t: triangles)
            t.visualize_normals(render_context);
}

void scene::overlay_reflection_vector_visualization() {
    if (visualize_reflection_vectors) {
        size_t step = triangles.size() / 250;
        step = max<size_t>(step, 1);

        for (size_t i = 0; i < triangles.size(); i += step)
            triangles[i].visualize_reflection_vectors(render_context);
    }
}
