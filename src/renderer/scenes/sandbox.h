#ifndef SANDBOX_H
#define SANDBOX_H

#include "scene.h"
#include "coordinate_system.h"
#include "mesh.h"

class sandbox : public scene {
public:
    sandbox();
    ~sandbox();

    void render(framebuffer &fb);

private:
    mesh line_thing;
    mesh icosahedron;
    coordinate_system cs;
};

#endif
