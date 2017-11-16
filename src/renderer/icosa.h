#ifndef ICOSA_H
#define ICOSA_H

#include "freecam_scene.h"
#include "mesh.h"

class icosa : public freecam_scene {
public:
    icosa();
    ~icosa();

    void render(framebuffer &fb);

private:
    mesh icosahedron;
};

#endif
