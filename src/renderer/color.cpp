#include "color.h"
#include "math_util.h"
#include <cmath>
#include <sstream>

using namespace std;

color::color() :
    color(0.0f, 0.0f, 0.0f, 1.0f) {
}

color::color(float r, float g, float b, float a) :
    r(r),
    g(g),
    b(b),
    a(a) {
}

float color::red() const {
    return r;
}

float color::green() const {
    return g;
}

float color::blue() const {
    return b;
}

float color::alpha() const {
    return a;
}

float &color::red() {
    return r;
}

float &color::green() {
    return g;
}

float &color::blue() {
    return b;
}

float &color::alpha() {
    return a;
}

color color::operator+(const color &rhs) const {
    return color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
}

color color::operator-(const color &rhs) const {
    return color(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
}

color color::operator*(const color &rhs) const {
    return color(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);
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

color &color::operator*=(const color &rhs) {
    r *= rhs.r;
    g *= rhs.g;
    b *= rhs.b;
    a *= rhs.a;

    return *this;
}

void color::superimpose(const color &rhs) {
    if (a <= 0.0f || rhs.a >= 1.0f) {
        r = rhs.r;
        g = rhs.g;
        b = rhs.b;
    }
    else {
#if FMA_OPTIMIZE_COLOR_SUPERIMPOSE
        float opaqueness = 1.0f - rhs.a;
        r = fma(opaqueness, r, rhs.a * rhs.r);
        g = fma(opaqueness, g, rhs.a * rhs.g);
        b = fma(opaqueness, b, rhs.a * rhs.b);

#else
        float opaqueness = 1.0f - rhs.a;
        r = opaqueness * r + rhs.a * rhs.r;
        g = opaqueness * g + rhs.a * rhs.g;
        b = opaqueness * b + rhs.a * rhs.b;
#endif
    }

    a = 1.0f;
}

color color::superimpose(const color &lhs, const color &rhs) {
    color result = lhs;
    result.superimpose(rhs);
    return result;
}

bool color::operator==(const color &rhs) {
    return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
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
