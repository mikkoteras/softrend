#ifndef ALPHA_TEST_H
#define ALPHA_TEST_H

#include "scene.h"
#include "mesh.h"

class command_line;
class framebuffer;

class alpha_test : public scene {
public:
    alpha_test(const command_line &cl);
    ~alpha_test();

    void compose();
    
private:
    mesh backdrop;
    mesh red_icosa, green_icosa, blue_icosa;
};

#endif
