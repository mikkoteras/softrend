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
                auto_scene sc(cl.object_filename(), cl.verbose(), auto_scene::as_is);
                window win(cl.width(), cl.height());
                return win.run(sc);
            }
            else {
                demo sc;
                window win(cl.width(), cl.height());
                return win.run(sc);
            }
        }
    }
    catch (...) {
        std::cerr << "caught stray exception, aborting." << std::endl;
        return 1;
    }
}
