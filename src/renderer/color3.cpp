#include "color3.h"
#include "color4.h"
#include "math_util.h"
#include <cmath>
#include <iomanip>
#include <sstream>

using namespace std;

color3::color3() :
    color3(0.0f, 0.0f, 0.0f) {
}

color3::color3(float r, float g, float b) :
    r(r),
    g(g),
    b(b) {
}

color3::color3(const color4 &rhs) :
    r(rhs.red()),
    g(rhs.green()),
    b(rhs.blue()) {
}

color3::color3(const color3 &rhs) :
    r(rhs.r),
    g(rhs.g),
    b(rhs.b) {
}

color3::color3(color3 &&rhs) :
    r(rhs.r),
    g(rhs.g),
    b(rhs.b) {
}

const color3 &color3::operator=(const color3 &rhs) {
    r = rhs.r;
    g = rhs.g;
    b = rhs.b;
    
    return *this;
}

const color3 &color3::operator=(color3 &&rhs) {
    r = rhs.r;
    g = rhs.g;
    b = rhs.b;

    return *this;
}

float color3::red() const {
    return r;
}

float color3::green() const {
    return g;
}

float color3::blue() const {
    return b;
}

float &color3::red() {
    return r;
}

float &color3::green() {
    return g;
}

float &color3::blue() {
    return b;
}

color3 color3::operator+(const color3 &rhs) const {
    return color3(r + rhs.r, g + rhs.g, b + rhs.b);
}

color3 color3::operator-(const color3 &rhs) const {
    return color3(r - rhs.r, g - rhs.g, b - rhs.b);
}

color3 color3::operator*(const color3 &rhs) const {
    return color3(r * rhs.r, g * rhs.g, b * rhs.b);
}

color3 color3::operator*(float rhs) const {
    return color3(r * rhs, g * rhs, b * rhs);
}

color3 color3::operator/(float rhs) const {
    return color3(r / rhs, g / rhs, b / rhs);
}

color3 &color3::operator+=(const color3 &rhs) {
    r += rhs.r;
    g += rhs.g;
    b += rhs.b;

    return *this;
}

color3 &color3::operator-=(const color3 &rhs) {
    r -= rhs.r;
    g -= rhs.g;
    b -= rhs.b;

    return *this;
}

color3 &color3::operator*=(const color3 &rhs) {
    r *= rhs.r;
    g *= rhs.g;
    b *= rhs.b;

    return *this;
}

color3 color3::operator+(const color4 &rhs) const {
    float opaqueness = rhs.alpha();
    float transparency = 1 - opaqueness;
    
#if FMA_OPTIMIZE_COLOR_SUPERIMPOSE
    return color3(fma(opaqueness, rhs.red(), transparency * r),
                  fma(opaqueness, rhs.green(), transparency * g),
                  fma(opaqueness, rhs.blue(), transparency * b);
#else
    return color3(transparency * r + opaqueness * rhs.red(),
                  transparency * g + opaqueness * rhs.green(),
                  transparency * b + opaqueness * rhs.blue());
#endif
}

color3 &color3::operator+=(const color4 &rhs) {
    float opaqueness = rhs.alpha();
    float transparency = 1 - opaqueness;

#if FMA_OPTIMIZE_COLOR_SUPERIMPOSE
    r = fma(opaqueness, rhs.red(), transparency * r);
    g = fma(opaqueness, rhs.greeen(), transparency * g);
    b = fma(opaqueness, rhs.blue(), transparency * b);
#else
    r *= transparency;
    g *= transparency;
    b *= transparency;
    r += opaqueness * rhs.red();
    g += opaqueness * rhs.green();
    b += opaqueness * rhs.blue();
#endif

    return *this;
}

bool color3::operator==(const color3 &rhs) {
    return r == rhs.r && g == rhs.g && b == rhs.b;
}

void color3::clamp() {
    math::clamp<float>(r, 0.0f, 1.0f);
    math::clamp<float>(g, 0.0f, 1.0f);
    math::clamp<float>(b, 0.0f, 1.0f);
}

color3 color3::clamped() const {
    return color3(math::clamped<float>(r, 0.0f, 1.0f),
                  math::clamped<float>(g, 0.0f, 1.0f),
                  math::clamped<float>(b, 0.0f, 1.0f));
}

std::uint32_t color3::to_rgba() const {
    return (static_cast<std::uint32_t>(255.0f * r) << 24) |
           (static_cast<std::uint32_t>(255.0f * g) << 16) |
           (static_cast<std::uint32_t>(255.0f * b) << 8) |
           0xFF;
}

std::uint32_t color3::to_ffrgb() const {
    return 0xFF000000 |
           (static_cast<std::uint32_t>(255.0f * r) << 16) |
           (static_cast<std::uint32_t>(255.0f * g) << 8) |
           static_cast<std::uint32_t>(255.0f * b);
}

std::uint8_t color3::r_to_int() const {
    return static_cast<std::uint8_t>(255.0f * r);
}

std::uint8_t color3::g_to_int() const {
    return static_cast<std::uint8_t>(255.0f * g);
}

std::uint8_t color3::b_to_int() const {
    return static_cast<std::uint8_t>(255.0f * b);
}

std::string color3::to_string() const {
    
    std::uint32_t rint = static_cast<std::uint32_t>(math::clamped<int>(static_cast<int>(255.0f * r), 0, 255));
    std::uint32_t gint = static_cast<std::uint32_t>(math::clamped<int>(static_cast<int>(255.0f * g), 0, 255));
    std::uint32_t bint = static_cast<std::uint32_t>(math::clamped<int>(static_cast<int>(255.0f * b), 0, 255));

    uint32_t hex = (rint << 16) | (gint << 8) | bint;

    ostringstream result;
    result << "0x" << setbase(16) << setw(6) << setfill('0') << hex << "_rgb";
    return result.str();
}

color3 operator*(float lhs, const color3 &rhs) {
    return rhs * lhs;
}

color3 operator"" _rgb(unsigned long long rgb) {
    if ((rgb & 0xFF000000ull) != 0)
        throw color3_construction_exception();

    float r = ((rgb & 0x00FF0000ull) >> 16) / 255.0f;
    float g = ((rgb & 0x0000FF00ull) >> 8)  / 255.0f;
    float b =  (rgb & 0x000000FFull)        / 255.0f;

    return color3(r, g, b);
}
