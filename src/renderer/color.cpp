#include "color.h"
#include "math_util.h"
#include <sstream>

using namespace std;

color::color() :
    color(0.0f, 0.0f, 0.0f, 0.0f) {
}

color::color(float r, float g, float b, float a) :
    r(r),
    g(g),
    b(b),
    a(a) {
}

color::color(const color &c1, const color &c2, float weight) :
    r(math::clamped(0.0f, (1.0f - weight) * c1.r + weight * c2.r, 1.0f)),
    g(math::clamped(0.0f, (1.0f - weight) * c1.g + weight * c2.g, 1.0f)),
    b(math::clamped(0.0f, (1.0f - weight) * c1.b + weight * c2.b, 1.0f)),
    a(math::clamped(0.0f, (1.0f - weight) * c1.a + weight * c2.a, 1.0f)) {
}

color color::operator+(const color &rhs) const {
    return color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
}

color color::operator-(const color &rhs) const {
    return color(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
}

color color::operator*(float rhs) const {
    return color(r * rhs, g * rhs, b * rhs, a * rhs);
}

color color::operator/(float rhs) const {
    return color(r / rhs, g / rhs, b / rhs, a / rhs);
}

color &color::operator+=(const color &rhs) {
    r += rhs.r;
    g += rhs.g;
    b += rhs.b;
    a += rhs.a;
    
    return *this;
}

void color::clamp() {
    math::clamp<float>(r, 0.0f, 1.0f);
    math::clamp<float>(g, 0.0f, 1.0f);
    math::clamp<float>(b, 0.0f, 1.0f);
    math::clamp<float>(a, 0.0f, 1.0f);
}

uint32_t color::to_rgba() const {
    typedef uint8_t byte;
    return ((byte)(255.0f * r) << 24) | ((byte)(255.0f * g) << 16) | ((byte)(255.0f * b) << 8) | (byte)(255.0f * a);
}

uint32_t color::to_ffrgb() const {
    typedef uint8_t byte;
    return 0xFF000000 | ((byte)(255.0f * r) << 16) | ((byte)(255.0f * g) << 8) | (byte)(255.0f * b);
}

uint8_t color::r_to_int() const {
    return (uint8_t)(255.0f * r);
}

uint8_t color::g_to_int() const {
    return (uint8_t)(255.0f * g);
}

uint8_t color::b_to_int() const {
    return (uint8_t)(255.0f * b);
}

std::string color::to_string() const {
    ostringstream result;
    result << "(" << r << ", " << g << ", " << b << ", " << a << ")";
    return result.str();
}

color operator*(float lhs, const color &rhs) {
    return rhs * lhs;
}
