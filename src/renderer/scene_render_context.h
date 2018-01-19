#ifndef SCENE_RENDER_CONTEXT_H
#define SCENE_RENDER_CONTEXT_H

#include "math/vector.h"

class framebuffer;
class light_list; 
class scene;

struct scene_render_context {
    framebuffer *target;
    scene *parent_scene;
    math::vector3f eye;
    const light_list *light_sources;
    unsigned num_threads;
};

#endif
