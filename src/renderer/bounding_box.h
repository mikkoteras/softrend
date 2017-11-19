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

    void stretch(const math::vector3f &point); // stretch box so that to the argument point fits
    void stretch(const math::vector4f &point);
    void stretch(const bounding_box &other);

public:
    const bounding_box &operator*=(float s);
    const bounding_box &operator/=(float s);
    
public:
    const math::vector3f &min() const;
    const math::vector3f &max() const;
    
private:
    math::vector3f min_corner;
    math::vector3f max_corner;
};

#endif