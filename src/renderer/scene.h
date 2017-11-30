#ifndef SCENE_H
#define SCENE_H

#include "animation_clock.h"
#include "benchmark.h"
#include "bounding_box.h"
#include "light_list.h"
#include "matrix.h"
#include "material_library.h"
#include "scene_info.h"
#include "vector.h"
#include <string>
#include <vector>

class light;
class framebuffer;

class scene {
public:
    scene();
    virtual ~scene();

public: // for window
    virtual void prerender(const framebuffer &fb);
    virtual void render(framebuffer &fb) = 0;
    virtual void key_down_event(int sdl_keycode, bool ctrl_is_down);
    virtual void mouse_move_event(int delta_x, int delta_y, bool left_button_is_down);
    virtual void mouse_wheel_event(int delta_x, int delta_y);

public:
    scene_info &get_scene_info();
    benchmark &get_benchmark();
    
public:
    math::matrix4x4f world_to_view();
    material_library &materials();
    const bounding_box &visible_volume() const;
    const light_list &light_sources() const;
    light_list &light_sources();
    const math::vector3f &get_eye_position() const;

public:
    void start();
    void stop();
    virtual bool stopped() const;

protected:
    void set_eye_position(const math::vector3f &position);
    void set_eye_direction(const math::vector3f &direction);           // option 1
    void set_eye_reference_point(const math::vector3f &look_at_point); // option 2
    void set_eye_orientation(const math::vector3f &up_direction);
    void set_view_to_view_plane_distance(float distance);
    
protected:
    animation_clock clock;
    material_library mat_lib;
    
private:
    math::vector3f eye_position;
    math::vector3f eye_direction;
    math::vector3f eye_up;
    float viewing_distance;
    math::matrix4x4f world_to_view_matrix;
    bool world_to_view_matrix_dirty;
    bounding_box framebuffer_visible_volume;
    light_list lights;
    
private:
    bool stop_requested;
    scene_info info;
    benchmark mark;
};

#endif
