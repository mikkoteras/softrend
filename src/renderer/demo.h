#ifndef DEMO_H
#define DEMO_H

#include "scene.h"
#include "color.h"
#include "coordinate_system.h"
#include "mesh.h"

class demo : public scene {
public:
    demo();
    ~demo();

    void prerender(framebuffer &fb) override;
    void compose() override;

    void render_dot_curve(framebuffer &fb);
    void render_line_spiral(framebuffer &fb);
    
    void compose_fern_still();
    void compose_fern_3d();

    void key_down_event(int sdl_keycode, bool ctrl_is_down) override;

private:
    void create_fern();
    void create_fern_recursive(const math::vector3f &root, const math::vector3f &tip, int generations);

private:
    enum stage_t {
        dot_curve,
        line_spiral,
        fern_still,
        fern_3d,
        min_stage = dot_curve,
        max_stage = fern_3d
    };

    stage_t stage = dot_curve;
    bool show_coord_sys = false;

    mesh fern;
};

#endif

