#ifndef COORDINATE_SYSTEM_H
#define COORDINATE_SYSTEM_H

#include "mesh.h"

class color3;
class framebuffer;
class scene;

class coordinate_system {
public:
    coordinate_system() = delete;
    coordinate_system(scene *parent_scene);
    coordinate_system(scene *parent_scene, const color3 &x, const color3 &y, const color3 &z);
    ~coordinate_system();

    mesh &get_mesh();
    const mesh &get_mesh() const;

private:
    mesh m;
};

#endif
