#include "window.h"
#include "auto_scene.h"
#include "command_line.h"
#include "demo.h"
#include <iostream>

int main(int argc, char *argv[]) {
    try {
        command_line cl(argc, argv);

        if (cl.valid()) {
            if (cl.scene_mode() == command_line::auto_scene) {
                auto_scene sc(cl, auto_scene::bounding_box_touches_origin);
                return window(cl, sc).run();
            }
            else {
                if (cl.verbose())
                    std::cout << "scene: " << cl.scene_name() << std::endl;
                
                if (cl.scene_name() == "demo") {
                    demo sc(cl);
                    return window(cl, sc).run();
                }
            }
        }

        return 1;
    }
    catch (...) {
        std::cerr << "caught stray exception, aborting." << std::endl;
        return 1;
    }
}
