#include "window.h"
#include "dnahelix.h"
#include "icosa.h"
#include "teapot.h"
#include "touareg.h"
#include <QtWidgets/QApplication>
#include <memory>

using namespace std;

int main(int argc, char *argv[]) {
    int w = 1600, h = 900;
    unique_ptr<scene> sc;

    // TODO: add argument handling that's a little less half-baked
    if (argc >= 2) {
        if (string(argv[1]) == "--scene=teapot")
            sc = unique_ptr<scene>(new teapot);
        else if (string(argv[1]) == "--scene=icosa")
            sc = unique_ptr<scene>(new icosa);
        else if (string(argv[1]) == "--scene=dnahelix")
            sc = unique_ptr<scene>(new dnahelix);
    }
    else
        sc = unique_ptr<scene>(new teapot);

    QApplication app(argc, argv);
    window win(sc.get(), w, h, nullptr);
    return app.exec();
}

