#ifndef SCENE_RENDER_CONTEXT_H
#define SCENE_RENDER_CONTEXT_H

#include "framebuffer.h"
#include "light_list.h"
#include "scene.h"
#include "math/vector.h"

struct scene_render_context {
    framebuffer *target;
    scene *parent_scene;
    math::vector3f eye;
    const light_list *light_sources;
    size_t num_threads;
};

#endif
