#include "bounding_box.h"
#include "vector_util.h"
#include <cmath>
#include <utility>

using namespace math;
using namespace std;

bounding_box::bounding_box(const math::vector3f &p) :
    min_corner(p),
    max_corner(p) {
}

bounding_box::bounding_box(const math::vector4f &p) :
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

void bounding_box::stretch(const math::vector3f &point) {
    min_corner = elementwise_min(min_corner, point);
    max_corner = elementwise_max(max_corner, point);
}

void bounding_box::stretch(const math::vector4f &point) {
    stretch(point.dehomo());
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

const math::vector3f &bounding_box::min() const {
    return min_corner;
}

const math::vector3f &bounding_box::max() const {
    return max_corner;
}
