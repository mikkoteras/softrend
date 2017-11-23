#ifndef DEMO_H
#define DEMO_H

#include "scene.h"
#include "mesh.h"

class demo : public scene {
public:
    demo();
    ~demo();

    void render(framebuffer &fb) override;

    void render_fern_still(framebuffer &fb);
    void render_fern_3d(framebuffer &fb);

    void key_down_event(int sdl_keycode, bool ctrl_is_down) override;

private:
    void create_fern();
    void create_fern_recursive(const math::vector3f &root,
                               const color &root_color,
                               float len, float phi, float theta, int generations);
    
private:
    enum stage_t { fern_still, fern_3d,
                   min_stage=fern_still, max_stage=fern_3d };
    stage_t stage = fern_still;
        
    mesh fern;
};

#endif
