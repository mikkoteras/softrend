#ifndef COLOR_H
#define COLOR_H

#include <cstdint>
#include <string>

class color {
public:
    color();
    color(float r, float g, float b, float a);
    color(const color &c1, const color &c2, float weight); // Interpolate between two colors

    color operator+(const color &rhs) const;
    color operator-(const color &rhs) const;
    color operator*(const color &rhs) const;
    color operator*(float rhs) const;
    color operator/(float rhs) const;
    color &operator+=(const color &rhs);

    void clamp();
    
    std::uint32_t to_rgba() const;
    std::uint32_t to_ffrgb() const;
    std::uint8_t r_to_int() const;
    std::uint8_t g_to_int() const;
    std::uint8_t b_to_int() const;

    std::string to_string() const;
    
private:
    float r, g, b, a;
};

color operator*(float lhs, const color &rhs);

#endif
