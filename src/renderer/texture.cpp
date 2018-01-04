#include "texture.h"
#include "lodepng.h"
#include "math_detail.h"
#include <cmath>
#include <iostream>

using namespace math;
using namespace std;

texture::texture() :
    width(0),
    height(0),
    max_x(-1),
    max_y(-1),
    transparency(false) {
}

texture::texture(const std::vector<unsigned char> &rgba, int w, int h) :
    width(w),
    height(h),
    max_x(w - 1),
    max_y(h - 1),
    transparency(false),
    pixels() {

    pixels.reserve(width * height);

    for (unsigned i = 0; i < rgba.size(); i += 4) {
        pixels.push_back(color4(rgba[i] / 255.0f,
                                rgba[i + 1] / 255.0f,
                                rgba[i + 2] / 255.0f,
                                rgba[i + 3] / 255.0f));
        transparency |= rgba[i + 3] < 255;
    }
}

texture::texture(const texture &rhs) :
    width(rhs.width),
    height(rhs.height),
    max_x(rhs.max_x),
    max_y(rhs.max_y),
    transparency(rhs.transparency),
    pixels(rhs.pixels) {
}

texture::texture(texture &&rhs) :
    width(rhs.width),
    height(rhs.height),
    max_x(rhs.max_x),
    max_y(rhs.max_y),
    transparency(rhs.transparency),
    pixels(move(rhs.pixels)) {
}

texture::~texture() {
}

const texture &texture::operator=(const texture &rhs) {
    if (&rhs != this) {
        width = rhs.width;
        height = rhs.height;
        max_x = rhs.max_x;
        max_y = rhs.max_y;
        transparency = rhs.transparency;
        pixels = rhs.pixels;
    }

    return *this;
}

texture &texture::operator=(texture &&rhs) {
    width = rhs.width;
    height = rhs.height;
    max_x = rhs.max_x;
    max_y = rhs.max_y;
    transparency = rhs.transparency;
    pixels = move(rhs.pixels);

    return *this;
}

int texture::pixel_width() const {
    return width;
}

int texture::pixel_height() const {
    return height;
}

bool texture::has_transparency() const {
    return transparency;
}

texture texture::load_png(const std::string &filename) {
    std::vector<unsigned char> rgba;
    unsigned width, height;
    unsigned error = lodepng::decode(rgba, width, height, filename);

    if (error) {
        std::cout << "failed to load " << filename << ", error = " << error << std::endl;
        throw texture_error();
    }
    else
        return texture(rgba, width, height);
}

int texture::index_at_point(const vector2f &uv) const {
    return index_at_point(uv[0], uv[1]);
}

int texture::index_at_point(float u, float v) const {
    // Normalize coords to get wraparound effect when
    // a coordinate <0 or >1.
    int xi = (u - math::detail::floor<float>(u)) * max_x;
    int yi = (1.0f - (v - math::detail::floor<float>(v))) * max_y;
    return yi * width + xi;
}

const color4 &texture::at(const vector2f &uv) const {
    return at(uv[0], uv[1]);
}

const color4 &texture::at(float u, float v) const {
    // Normalize coords to get wraparound effect when
    // a coordinate <0 or >1.
    int xi = (u - math::detail::floor<float>(u)) * max_x;
    int yi = (1.0f - (v - math::detail::floor<float>(v))) * max_y;
    return pixels[yi * width + xi];
}

const color4 &texture::at(int i) const {
    return pixels[i];
}
