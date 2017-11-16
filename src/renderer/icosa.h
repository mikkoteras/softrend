#ifndef ICOSA_H
#define ICOSA_H

#include "scene.h"
#include "mesh.h"

class icosa : public scene {
public:
    icosa();
    ~icosa();

    void render(framebuffer &fb);

private:
    mesh icosahedron;
};

#endif
