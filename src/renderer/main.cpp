#include "window.h"
#include "auto_scene.h"
#include <iostream>

int main(int argc, char *argv[]) {
    try {
        int w = 1600, h = 900;
        auto_scene sc("assets/teapot/tpot.obj", true, auto_scene::bounding_box_touches_origin);
        window win(w, h);
        return win.run(sc);
    }
    catch (...) {
        std::cerr << "caught stray exception, aborting." << std::endl;
        return 1;
    }
}
