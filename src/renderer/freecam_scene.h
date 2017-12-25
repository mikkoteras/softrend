#ifndef FREECAM_SCENE
#define FREECAM_SCENE

#include "scene.h"
#include "vector.h"

class freecam_scene : public scene {
public:
    freecam_scene(float initial_eye_radius = 1.0f);
    ~freecam_scene();

    void set_point_of_interest(const math::vector3f &p);
    void prerender(framebuffer&) override;

    void key_down_event(int sdk_keycode, bool ctrl_is_down) override;
    void mouse_move_event(int delta_x, int delta_y, bool left_button_is_down) override;
    void mouse_wheel_event(int delta_x, int delta_y) override;

private:
    float eye_azimuth_angle, eye_polar_angle, eye_radius;
    float eye_twist_angle;
    math::vector3f point_of_interest;
};

#endif
