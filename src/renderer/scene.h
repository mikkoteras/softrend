#ifndef SCENE_H
#define SCENE_H

#include "animation_clock.h"
#include "light_list.h"
#include "matrix.h"
#include "material_library.h"
#include "vector.h"
#include <memory>
#include <vector>

class light;
class framebuffer;

class scene {
public:
    scene();
    virtual ~scene();

    void start();
    void stop();
    virtual bool stopped() const;
    
    virtual void render(framebuffer &fb) = 0;

    virtual void key_down_event(int sdl_keycode, bool ctrl_is_down);
    virtual void mouse_move_event(int delta_x, int delta_y, bool left_button_is_down);
    virtual void mouse_wheel_event(int delta_x, int delta_y);
    
public:
    math::matrix4x4f world_to_view();
    material_library &materials();
    color shade(const math::vector3f &surface_normal) const;

protected:
    const math::vector3f &get_eye_position() const;
    
    void set_eye_position(const math::vector3f &position);
    void set_eye_direction(const math::vector3f &direction);           // option 1
    void set_eye_reference_point(const math::vector3f &look_at_point); // option 2
    void set_eye_orientation(const math::vector3f &up_direction);
    void set_view_to_view_plane_distance(float distance);

    void add_ambient_light(const color &light_color);
    void add_directional_light(const math::vector3f &direction, const color &light_color);
    void add_light(std::unique_ptr<light> l);
    
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
    light_list lights;

private:
    bool stop_requested;
};

#endif
