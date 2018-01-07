#ifndef DEMO_H
#define DEMO_H

#include "scene.h"
#include "coordinate_system.h"
#include "mesh.h"

class command_line;

class demo : public scene {
public:
    demo(const command_line &cl);
    ~demo();

    void prerender(framebuffer &fb) override;
    void compose() override;

    void render_dot_curve(framebuffer &fb);
    void render_line_spiral(framebuffer &fb);
    
    void compose_fern();
    void compose_slab_demo();

    void key_down_event(int sdl_keycode, bool ctrl_is_down) override;

private:
    void create_fern();
    void create_fern_recursive(const math::vector3f &root, const math::vector3f &tip, int generations);

private:
    void hide_all_meshes();

private:
    enum stage_t {
        dot_curve,
        line_spiral,
        fern_still,
        fern_3d,
        slab_demo,
        min_stage = dot_curve,
        max_stage = slab_demo
    };

    stage_t stage = dot_curve;

    mesh fern;
    mesh slab;
};

#endif
