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
    void mouse_move_event(int delta_x, int delta_y) override;
    void mouse_wheel_event(int delta) override;

private:
    bool show_coords;
    coordinate_system coords;
};

#endif
