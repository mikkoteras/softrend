#ifndef FREECAM_SCENE
#define FREECAM_SCENE

#include "scene.h"
#include "coordinate_system.h"

class freecam_scene : public scene {
public:
    freecam_scene();
    virtual ~freecam_scene();

    void render(framebuffer &fb) override;
    
    void key_down_event(int sdk_keycode, bool ctrl_is_down) override;
    void mouse_move_event(int delta_x, int delta_y, bool left_button_is_down) override;
    void mouse_wheel_event(int delta) override;

private:
    bool show_coords;
    coordinate_system coords;

    float eye_azimuth_angle, eye_polar_angle, eye_radius;
    float eye_twist_angle;

//    float eye_xz_orbit, eye_elevation;
};

#endif
