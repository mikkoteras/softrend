#ifndef FREECAM_SCENE
#define FREECAM_SCENE

#include "scene.h"
#include "math/math_detail.h"
#include "math/vector.h"

class command_line;

class freecam_scene : public scene {
public:
    freecam_scene(const command_line &cl, float initial_eye_radius = 1.0f);
    ~freecam_scene();

    void set_point_of_interest(const math::vector3f &p);
    void compose() override;

    void key_down_event(int sdk_keycode, bool ctrl_is_down) override;
    void mouse_move_event(int delta_x, int delta_y, bool left_button_is_down) override;
    void mouse_wheel_event(int delta_x, int delta_y) override;

private:
    float eye_azimuth_angle = math::detail::pi<float>();
    float eye_polar_angle = 0.0f;
    float eye_radius;
    float eye_twist_angle = 0.0f;
    math::vector3f point_of_interest = math::vector3f{0.0f, 0.0f, 0.0f};
    const float window_width, window_height;
};

#endif
