#ifndef SCENE_H
#define SCENE_H

#include "animation_clock.h"
#include "benchmark.h"
#include "bounding_box.h"
#include "coordinate_system.h"
#include "light_list.h"
#include "line.h"
#include "material_library.h"
#include "scene_info.h"
#include "scene_render_context.h"
#include "thread_pool.h"
#include "triangle.h"
#include "types.h"
#include "math/matrix.h"
#include "math/vector.h"
#include <condition_variable>
#include <string>
#include <thread>
#include <vector>

class command_line;
class framebuffer;
class light;
class mesh;

class scene {
public:
    scene(const command_line &cl);
    virtual ~scene();

    scene() = delete;

public: // for runtime options and info
    void cycle_shading_model();
    void set_shading_model(shading_model_t rm);
    shading_model_t get_shading_model() const;
    void set_normal_visualization(bool setting);
    bool get_normal_visualization() const;
    void set_reflection_vector_visualization(bool setting);
    bool get_reflection_vector_visualization() const;
    void set_wireframe_visualization(bool setting);
    bool get_wireframe_visualization() const;
    void set_coordinate_system(bool setting);
    bool get_coordinate_system() const;
    double get_animation_time() const;

public: // for window render loop
    void render(framebuffer &fb);
    virtual void key_down_event(int sdl_keycode, bool ctrl_is_down);
    virtual void mouse_move_event(int delta_x, int delta_y, bool left_button_is_down);
    virtual void mouse_wheel_event(int delta_x, int delta_y);

public:
    scene_info &get_scene_info();
    benchmark &get_benchmark();

public: // for mesh
    int add_vertex(const math::vector3f &v);
    int add_vertex_normal(const math::vector3f &vn);
    int add_line(int v1, int v2, const color3 &c1, const color3 &c2);
    int add_triangle(int vi1, int vi2, int vi3,
                     int ni1, int ni2, int ni3,
                     const math::vector2f &uv1, const math::vector2f &uv2, const math::vector2f &uv3,
                     const material *mat);
    int add_triangle(int vi1, int vi2, int vi3, int ni1, int ni2, int ni3, const material *mat);
    int add_mesh(mesh *caller);

public:
    const math::matrix4x4f &world_to_view();
    material_library &materials();
    const light_list &light_sources() const;
    light_list &light_sources();
    const bounding_box &visible_volume() const;
    const math::vector3f &get_eye_position() const;
    const math::vector4f *local_coordinate_data() const;
    const math::vector3f *world_coordinate_data() const;
    const math::vector3f *world_normal_data() const;
    const math::vector3f *view_coordinate_data() const;

public:
    int get_vertex_count() const;
    int get_normal_count() const;
    int get_visible_line_count() const;
    int get_visible_triangle_count() const;

public:
    void start();
    void stop();
    virtual bool stopped() const;

protected: // derived class interface
    virtual void compose() = 0;
    virtual void prerender(const scene_render_context &scene_context);
    virtual void postrender(const scene_render_context &scene_context);

protected: // for composition
    void set_eye_position(const math::vector3f &position);
    void set_eye_direction(const math::vector3f &direction);           // option 1
    void set_eye_reference_point(const math::vector3f &look_at_point); // option 2
    void set_eye_orientation(const math::vector3f &up_direction);
    void set_fov(float fov_radians);

private: // render helpers
    void construct_world_to_view(const framebuffer &fb);
    void compute_visible_volume(const framebuffer &fb);
    void transform_coordinates();
    void render_lines(framebuffer &fb);
    void render_triangles();
    void prepare_triangles_for_render_threaded(size_t thread_index);
    void render_triangles_threaded(size_t thread_index);
    void overlay_wireframe_visualization();
    void overlay_normal_visualization();
    void overlay_reflection_vector_visualization();

    void foo(size_t bar);

protected:
    animation_clock clock;
    material_library mat_lib;
    light_list lights;

protected: // runtime configurable parameters
    shading_model_t shading_model = phong;
    bool visualize_normals = false;
    bool visualize_reflection_vectors = false;
    bool visualize_wireframe = false;

private:
    struct triangle_distance { // for painter's algortihm
        unsigned triangle_index;
        float z_coordinate;
        bool operator<(const triangle_distance &rhs) { return z_coordinate < rhs.z_coordinate; }
    };

private:
    std::vector<math::vector4f> local_coordinates;
    std::vector<math::vector4f> local_normals;
    std::vector<triangle> triangles;
    std::vector<triangle_distance> triangle_order;
    std::vector<line> lines;
    std::vector<mesh*> meshes;

    std::vector<math::vector3f> world_coordinates;
    std::vector<math::vector3f> world_normals;
    std::vector<math::vector3f> view_coordinates;

private:
    int num_visible_lines;
    int num_visible_triangles;

private: // TODO: refactor this into class projection or something
    math::vector3f eye_position = math::vector3f{0.0f, 0.0f, 1.0f};
    math::vector3f eye_direction = math::vector3f{0.0f, 0.0f, -1.0f};
    math::vector3f eye_up = math::vector3f{0.0f, 1.0f, 0.0f};
    float fov;
    math::matrix4x4f world_to_view_matrix = math::matrix4x4f::identity();
    bounding_box framebuffer_visible_volume;

private:
    coordinate_system coords;
    
private:
    scene_info info;
    benchmark mark;

private:
    const size_t num_threads;
    thread_pool threads;
    scene_render_context render_context;
    bool stop_requested = false;
};

#endif
