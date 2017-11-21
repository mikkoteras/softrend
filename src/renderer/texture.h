#ifndef TEXTURE_H
#define TEXTURE_H

#include "color.h"
#include <vector>

class texture_error {};

class texture {
public:
    texture();
    texture(const std::vector<unsigned char> &rgba, int w, int h);
    texture(const texture &rhs);
    texture &operator=(texture &&rhs);

    ~texture();

    texture(texture &&rhs);
    const texture &operator=(const texture &rhs);

    int pixel_width() const;
    int pixel_height() const;

public:
    static texture load_png(const std::string &filename);

public:
    const color &at(float x, float y) const;

private:
    int width, height;
    int max_x, max_y;
    std::vector<color> pixels;
};

#endif
