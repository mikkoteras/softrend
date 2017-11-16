#ifndef AUTO_SCENE
#define AUTO_SCENE

#include "scene.h"
#include "mesh.h"

class auto_scene : public scene {
public:
    auto_scene();
    ~auto_scene();

private:
    mesh m;
};

#endif
