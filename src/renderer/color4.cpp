#include "color4.h"
#include "color3.h"
#include "math/math_util.h"
#include <cmath>
#include <iomanip>
#include <sstream>

using namespace std;

color4::color4() :
    color4(0.0f, 0.0f, 0.0f, 1.0f) {
}

color4::color4(float r, float g, float b, float a) :
    r(r),
    g(g),
    b(b),
    a(a) {
}

color4::color4(const color3 &rhs, float a) :
    r(rhs.red()),
    g(rhs.green()),
    b(rhs.blue()),
    a(a) {
}

color4::color4(const color4 &rhs) :
    r(rhs.r),
    g(rhs.g),
    b(rhs.b),
    a(rhs.a) {
}

color4::color4(color4 &&rhs) :
    r(rhs.r),
    g(rhs.g),
    b(rhs.b),
    a(rhs.a) {
}

const color4 &color4::operator=(const color4 &rhs) {
    r = rhs.r;
    g = rhs.g;
    b = rhs.b;
    a = rhs.a;

    return *this;
}

const color4 &color4::operator=(color4 &&rhs) {
    r = rhs.r;
    g = rhs.g;
    b = rhs.b;
    a = rhs.a;

    return *this;
}

float color4::red() const {
    return r;
}

float color4::green() const {
    return g;
}

float color4::blue() const {
    return b;
}

float color4::alpha() const {
    return a;
}

float &color4::red() {
    return r;
}

float &color4::green() {
    return g;
}

float &color4::blue() {
    return b;
}

float &color4::alpha() {
    return a;
}

color4 color4::operator*(const color3 &rhs) const {
    return color4(r * rhs.red(), g * rhs.green(), b * rhs.blue(), a);
}

color4 &color4::operator*=(const color3 &rhs) {
    r *= rhs.red();
    g *= rhs.green();
    b *= rhs.blue();
    
    return *this;
}

bool color4::operator==(const color4 &rhs) {
    return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
}

void color4::clamp() {
    math::clamp<float>(r, 0.0f, 1.0f);
    math::clamp<float>(g, 0.0f, 1.0f);
    math::clamp<float>(b, 0.0f, 1.0f);
    math::clamp<float>(a, 0.0f, 1.0f);
}

color4 color4::clamped() const {
    return color4(math::clamped<float>(r, 0.0f, 1.0f),
                  math::clamped<float>(g, 0.0f, 1.0f),
                  math::clamped<float>(b, 0.0f, 1.0f),
                  math::clamped<float>(a, 0.0f, 1.0f));
}

std::string color4::to_string() const {
    uint32_t rint = static_cast<uint32_t>(math::clamped<int>(static_cast<int>(255.0f * r), 0, 255));
    uint32_t gint = static_cast<uint32_t>(math::clamped<int>(static_cast<int>(255.0f * g), 0, 255));
    uint32_t bint = static_cast<uint32_t>(math::clamped<int>(static_cast<int>(255.0f * b), 0, 255));
    uint32_t aint = static_cast<uint32_t>(math::clamped<int>(static_cast<int>(255.0f * a), 0, 255));

    uint32_t hex = (rint << 24) | (gint << 16) | (bint << 8) | aint;

    ostringstream result;
    result << "0x" << setbase(16) << setw(8) << setfill('0') << hex << "_rgba";
    return result.str();
}

color4 operator"" _rgba(unsigned long long rgba) {
    float r = ((rgba & 0xFF000000ull) >> 24) / 255.0f;
    float g = ((rgba & 0x00FF0000ull) >> 16) / 255.0f;
    float b = ((rgba & 0x0000FF00ull) >> 8)  / 255.0f;
    float a =  (rgba & 0x000000FFull)        / 255.0f;

    return color4(r, g, b, a);
}
