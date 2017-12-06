#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "vector.h"

class bounding_box {
public:
    bounding_box() = delete;
    bounding_box(const math::vector3f &p); // box will consist only of p.
    bounding_box(const math::vector4f &p);
    bounding_box(const bounding_box &rhs);
    bounding_box(bounding_box &&rhs);
    const bounding_box &operator=(const bounding_box &rhs);
    bounding_box &operator=(bounding_box &&rhs);

    ~bounding_box();

public:
    float width() const;
    float height() const;
    float depth() const;
    float max_semiaxis() const;

    void stretch(const math::vector3f &p); // stretch box so as to contain p
    void stretch(const math::vector4f &p);
    void stretch(const bounding_box &other);

public:
    const bounding_box &operator*=(float s);
    const bounding_box &operator/=(float s);

public:
    const math::vector3f &min() const;
    const math::vector3f &max() const;
    bool contains(const math::vector3f &p) const;

    bool clip(const math::vector4f &v1, const math::vector4f &v2) const; // lines
    bool clip(const math::vector3f &v1, const math::vector3f &v2, const math::vector3f &v3) const; // triangles
    bool clip(const math::vector4f &v1, const math::vector4f &v2, const math::vector4f &v3) const; // triangles

private:
    math::vector3f min_corner;
    math::vector3f max_corner;
};

#endif
