#ifndef COORDINATE_SYSTEM_H
#define COORDINATE_SYSTEM_H

#include "mesh.h"

class color;
class framebuffer;
class scene;

class coordinate_system {
public:
    coordinate_system();
    coordinate_system(const color &x, const color &y, const color &z);
    ~coordinate_system();

    void render(scene &sc, framebuffer &fb);
    
private:
    mesh m;
};

#endif
