#ifndef VERTEX_H
#define VERTEX_H

#include "color.h"
#include "vector.h"

class vertex {
public:
    vertex();
    vertex(int x, int y, const color &c);
    vertex(int x, int y, float z, const color &c);
    template<typename float_t> vertex(const math::vector<float_t, 2> &vec, const color &c);
    template<typename float_t> vertex(const math::vector<float_t, 3> &vec, const color &c);
    template<typename float_t> vertex(const math::vector<float_t, 4> &vec, const color &c);
    
    int x() const;
    int y() const;
    float z() const;
    const color &c() const;
        
private:
    int xc, yc;
    float zc;
    color col;
};

template<typename float_t> vertex::vertex(const math::vector<float_t, 2> &vec, const color &c) :
    xc(vec.x()),
    yc(vec.y()),
    zc(0.0f),
    col(c) {
}

template<typename float_t> vertex::vertex(const math::vector<float_t, 3> &vec, const color &c) :
    xc(vec.x()),
    yc(vec.y()),
    zc(vec.z()),
    col(c) {
}

template<typename float_t> vertex::vertex(const math::vector<float_t, 4> &vec, const color &c) :
    xc(vec.x() / vec.h()),
    yc(vec.y() / vec.h()),
    zc(vec.z() / vec.h()),
    col(c) {
}

#endif
