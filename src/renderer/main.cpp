#include "window.h"
#include "alpha_test.h"
#include "auto_scene.h"
#include "command_line.h"
#include "demo.h"
#include <iostream>

int main(int argc, char *argv[]) {
    try {
        command_line cl(argc, argv);

        if (cl.valid()) {
            window win(cl.width(), cl.height());
            
            if (cl.scene_mode() == command_line::auto_scene) {
                auto_scene sc(cl.object_filename(), cl.verbose(), auto_scene::bounding_box_touches_origin);
                return win.run(sc);
            }
            else {
                if (cl.verbose())
                    std::cout << "scene: " << cl.scene_name() << std::endl;
                
                if (cl.scene_name() == "demo") {
                    demo sc(cl.verbose());
                    return win.run(sc);
                }
                else if (cl.scene_name() == "alpha") {
                    alpha_test sc(cl.verbose());
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
