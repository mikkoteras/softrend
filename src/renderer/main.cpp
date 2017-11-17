#include "window.h"
#include "auto_scene.h"
#include "scenes/icosa.h"
#include <iostream>

int main(int argc, char *argv[]) {
    try {
        int w = 1600, h = 900;
        icosa s;
        window win(w, h);
        return win.run(s);
    }
    catch (...) {
        std::cerr << "caught stray exception, aborting." << std::endl;
        return 1;
    }
}

