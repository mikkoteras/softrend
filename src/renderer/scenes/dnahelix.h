#ifndef DNAHELIX_H
#define DNAHELIX_H

#include "color.h"
#include "mesh.h"
#include "scene.h"
#include "texture.h"

class dnahelix : public scene {
public:
    dnahelix();
    ~dnahelix();

    void render(framebuffer &fb);

private:
    mesh dnahelix_mesh;
};

#endif
