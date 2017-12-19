#ifndef ALPHA_TEST_H
#define ALPHA_TEST_H

#include "scene.h"
#include "mesh.h"

class framebuffer;

class alpha_test : public scene {
public:
    alpha_test(bool echo_comments);
    ~alpha_test();

    void render(framebuffer &fb);
    
private:
    mesh backdrop;
    mesh red_icosa, green_icosa, blue_icosa;
};

#endif
