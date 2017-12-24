#ifndef COORDINATE_SYSTEM_H
#define COORDINATE_SYSTEM_H

#include "mesh.h"

class color;
class framebuffer;
class scene;

class coordinate_system {
public:
    coordinate_system() = delete;
    coordinate_system(scene *parent_scene);
    coordinate_system(scene *parent_scene, const color &x, const color &y, const color &z);
    ~coordinate_system();

    mesh &get_mesh();
    const mesh &get_mesh() const;

private:
    mesh m;
};

#endif
