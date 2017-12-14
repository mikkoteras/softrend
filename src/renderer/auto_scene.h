#ifndef AUTO_SCENE
#define AUTO_SCENE

#include "freecam_scene.h"
#include "mesh.h"
#include <experimental/filesystem>

class auto_scene : public freecam_scene {
public:
    enum object_position { as_is, center_at_origin, bounding_box_touches_origin, center_at_unity };

    auto_scene(const std::experimental::filesystem::path &object_file, bool echo_comments, object_position pos);
    ~auto_scene();

    void key_down_event(int sdk_keycode, bool ctrl_is_down) override;

    void render(framebuffer &fb);

private:
    mesh object;
    float previous_render_time = 0.0f;
    float x_rotation_per_second = 0.0f;
    float y_rotation_per_second = 0.0f;
    float z_rotation_per_second = 0.0f;
    float x_rotation = 0.0f;
    float y_rotation = 0.0f;
    float z_rotation = 0.0f;
};

#endif
