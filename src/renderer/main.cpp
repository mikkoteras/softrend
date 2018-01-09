#include "window.h"
#include "auto_scene.h"
#include "command_line.h"
#include "demo.h"
#include <iostream>

int main(int argc, char *argv[]) {
    try {
        command_line cl(argc, argv);

        if (cl.valid()) {
            window win(cl.width(), cl.height());

            if (cl.verbose())
                std::cout << "using " << cl.rasterizer_threads() << " threads to rasterize." << std::endl;
            
            if (cl.scene_mode() == command_line::auto_scene) {
                auto_scene sc(cl, auto_scene::bounding_box_touches_origin);
                return win.run(sc);
            }
            else {
                if (cl.verbose())
                    std::cout << "scene: " << cl.scene_name() << std::endl;
                
                if (cl.scene_name() == "demo") {
                    demo sc(cl);
                    return win.run(sc);
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
