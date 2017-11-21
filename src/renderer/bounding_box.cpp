#include "bounding_box.h"
#include "vector_util.h"
#include <cmath>
#include <utility>

using namespace math;
using namespace std;

bounding_box::bounding_box(const vector3f &p) :
    min_corner(p),
    max_corner(p) {
}

bounding_box::bounding_box(const vector4f &p) :
    bounding_box(p.dehomo()) {
}

bounding_box::bounding_box(const bounding_box &rhs) :
    min_corner(rhs.min_corner),
    max_corner(rhs.max_corner) {
}

bounding_box::bounding_box(bounding_box &&rhs) :
    min_corner(rhs.min_corner),
    max_corner(rhs.max_corner) {
}

const bounding_box &bounding_box::operator=(const bounding_box &rhs) {
    min_corner = rhs.min_corner;
    max_corner = rhs.max_corner;
    return *this;
}

bounding_box &bounding_box::operator=(bounding_box &&rhs) {
    min_corner = rhs.min_corner;
    max_corner = rhs.max_corner;
    return *this;
}
    
bounding_box::~bounding_box() {
}

float bounding_box::width() const {
    return max_corner.x() - min_corner.x();
}

float bounding_box::height() const {
    return max_corner.y() - min_corner.y();
}

float bounding_box::depth() const {
    return max_corner.z() - min_corner.z();
}

float bounding_box::max_semiaxis() const {
    float result = 0.0f;

    for (int i = 0; i < 3; ++i) {
        result = std::max(result, fabs(min_corner[i]));
        result = std::max(result, fabs(max_corner[i]));
    }
    
    return result;
}

void bounding_box::stretch(const vector3f &p) {
    min_corner = elementwise_min(min_corner, p);
    max_corner = elementwise_max(max_corner, p);
}

void bounding_box::stretch(const vector4f &p) {
    stretch(p.dehomo());
}

void bounding_box::stretch(const bounding_box &other) {
    min_corner = elementwise_min(min_corner, other.min_corner);
    max_corner = elementwise_max(max_corner, other.max_corner);
}

const bounding_box &bounding_box::operator*=(float s) {
    min_corner *= s;
    max_corner *= s;
    return *this;
}

const bounding_box &bounding_box::operator/=(float s) {
    min_corner /= s;
    max_corner /= s;
    return *this;
}

const vector3f &bounding_box::min() const {
    return min_corner;
}

const vector3f &bounding_box::max() const {
    return max_corner;
}

bool bounding_box::contains(const vector3f &p) const {
    return
        min_corner.x() <= p.x() && min_corner.y() <= p.y() && min_corner.z() <= p.z() &&
        p.x() <= max_corner.x() && p.y() <= max_corner.y() && p.z() <= max_corner.z();
}

#include <iostream>

bool bounding_box::clip(const vector4f &v1, const vector4f &v2) const {
    // TODO: why not an intersect test while we're at it?
    if (v1.x() < min_corner.x() && v2.x() < min_corner.x())
        return true;
    else if (v1.y() < min_corner.y() && v2.y() < min_corner.y())
        return true;
    else if (v1.z() < min_corner.z() && v2.z() < min_corner.z())
        return true;
    else if (v1.x() > max_corner.x() && v2.x() > max_corner.x())
        return true;
    else if (v1.y() > max_corner.y() && v2.y() > max_corner.y())
        return true;
    else if (v1.z() > max_corner.z() && v2.z() > max_corner.z())
        return true;

    return false;
}

bool bounding_box::clip(const math::vector4f &v1, const vector4f &v2, const vector4f &v3) const {
    // TODO: why not an intersect test while we're at it?
    if (v1.x() < min_corner.x() && v2.x() < min_corner.x() && v3.x() < min_corner.x())
        return true;
    else if (v1.y() < min_corner.y() && v2.y() < min_corner.y() && v3.y() < min_corner.y())
        return true;
    else if (v1.z() < min_corner.z() && v2.z() < min_corner.z() && v3.z() < min_corner.z())
        return true;
    else if (v1.x() > max_corner.x() && v2.x() > max_corner.x() && v3.x() > max_corner.x())
        return true;
    else if (v1.y() > max_corner.y() && v2.y() > max_corner.y() && v3.y() > max_corner.y())
        return true;
    else if (v1.z() > max_corner.z() && v2.z() > max_corner.z() && v3.z() > max_corner.z())
        return true;

    return false;
}
