#ifndef TEAPOT_H
#define TEAPOT_H

#include "color.h"
#include "mesh.h"
#include "scene.h"
#include "texture.h"

class teapot : public scene {
public:
    teapot();
    ~teapot();

    void render(framebuffer &fb);

private:
    mesh teapot_mesh;
};

#endif
