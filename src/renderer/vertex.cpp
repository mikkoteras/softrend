#include "vertex.h"

using namespace math;

vertex::vertex() {
}

vertex::vertex(int x, int y, const color &c) :
    xc(x),
    yc(y),
    zc(0.0f),
    col(c) {
}

vertex::vertex(int x, int y, float z, const color &c) :
    xc(x),
    yc(y),
    zc(z),
    col(c) {
}

int vertex::x() const {
    return xc;
}

int vertex::y() const {
    return yc;
}

float vertex::z() const {
    return zc;
}

const color &vertex::c() const {
    return col;
}
