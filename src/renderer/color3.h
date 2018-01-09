#ifndef COLOR3_H
#define COLOR3_H

#include <cstdint>
#include <string>

class color4;

class color3_construction_exception{};

class color3 {
public:
    color3();
    color3(float r, float g, float b);
    color3(const color4 &rhs);
    
    color3(const color3 &rhs);
    color3(color3 &&rhs);
    const color3 &operator=(const color3 &rhs);
    const color3 &operator=(color3 &&rhs);

    float red() const;
    float green() const;
    float blue() const;
    float &red();
    float &green();
    float &blue();
    
    color3 operator+(const color3 &rhs) const;
    color3 operator-(const color3 &rhs) const;
    color3 operator*(const color3 &rhs) const;
    color3 operator*(float rhs) const;
    color3 operator/(float rhs) const;
    color3 &operator+=(const color3 &rhs);
    color3 &operator-=(const color3 &rhs);
    color3 &operator*=(const color3 &rhs);

    color3 operator+(const color4 &rhs) const;
    color3 &operator+=(const color4 &rhs);

    bool operator==(const color3 &rhs);
    bool operator!=(const color3 &rhs) { return !(*this == rhs); }

    void clamp();
    color3 clamped() const;

    std::uint32_t to_rgba() const;
    std::uint32_t to_ffrgb() const;
    std::uint8_t r_to_int() const;
    std::uint8_t g_to_int() const;
    std::uint8_t b_to_int() const;

    std::string to_string() const;

private:
    float r, g, b;
};

color3 operator*(float lhs, const color3 &rhs);
color3 operator"" _rgb(unsigned long long rgb);


#endif
