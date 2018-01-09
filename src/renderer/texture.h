#ifndef TEXTURE_H
#define TEXTURE_H

#include "color4.h"
#include "math/vector.h"
#include <vector>

class texture_error {};

class texture {
public:
    texture();
    texture(const std::vector<unsigned char> &rgba, int w, int h);
    texture(const texture &rhs);
    texture(texture &&rhs);

    ~texture();

    texture &operator=(texture &&rhs);
    const texture &operator=(const texture &rhs);

    int pixel_width() const;
    int pixel_height() const;
    bool has_transparency() const;

public:
    static texture load_png(const std::string &filename);

public:
    int index_at_point(const math::vector2f &uv) const;
    int index_at_point(float u, float v) const;

    const color4 &at(const math::vector2f &uv) const;
    const color4 &at(float u, float v) const;
    const color4 &at(int i) const;

private:
    int width, height;
    int max_x, max_y;
    bool transparency;
    std::vector<color4> pixels;
};

#endif
