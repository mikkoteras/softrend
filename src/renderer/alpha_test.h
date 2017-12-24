#ifndef ALPHA_TEST_H
#define ALPHA_TEST_H

#include "scene.h"
#include "mesh.h"

class framebuffer;

class alpha_test : public scene {
public:
    alpha_test(bool verbose);
    ~alpha_test();

    void compose();
    
private:
    mesh backdrop;
    mesh red_icosa, green_icosa, blue_icosa;
};

#endif
