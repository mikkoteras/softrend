#ifndef TOUAREG_H
#define TOUAREG_H

#include "scene.h"
#include "mesh.h"

class touareg : public scene {
public:
    touareg();
    ~touareg();

    void render(framebuffer &fb);

private:
    mesh car;
};

#endif
