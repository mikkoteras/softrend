#ifndef COLOR4_H
#define COLOR4_H

#include <cstdint>
#include <string>

class color3;

class color4 {
public:
    color4();
    color4(float r, float g, float b, float a);
    color4(const color3 &rhs, float a);

    color4(const color4 &rhs);
    color4(color4 &&rhs);
    const color4 &operator=(const color4 &rhs);
    const color4 &operator=(color4 &&rhs);
    
    float red() const;
    float green() const;
    float blue() const;
    float alpha() const;
    float &red();
    float &green();
    float &blue();
    float &alpha();

    color4 operator*(const color3 &rhs) const;
    color4 &operator*=(const color3 &rhs);
    
    bool operator==(const color4 &rhs);
    bool operator!=(const color4 &rhs) { return !(*this == rhs); }

    void clamp();
    color4 clamped() const;

    std::string to_string() const;

private:
    float r, g, b, a;
};

color4 operator"" _rgba(unsigned long long rgba);

#endif
