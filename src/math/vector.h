#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H

#include <initializer_list>

namespace math {
    template<typename float_t, int dim> class vector;
    template<typename float_t, int rows, int columns> class matrix;

    template<typename float_t, int dim> class vector {
    public:
        constexpr vector();
        vector(const vector &rhs);
        vector(vector &&rhs);
        constexpr vector(const std::initializer_list<float_t> &il);
        constexpr vector(const vector<float_t, dim - 1> &v, float_t h);

        constexpr float_t operator[](int i) const;
        constexpr float_t &operator[](int i);

        constexpr float_t x() const;
        constexpr float_t y() const;
        constexpr float_t z() const;
        constexpr float_t h() const;
        constexpr float_t &x();
        constexpr float_t &y();
        constexpr float_t &z();
        constexpr float_t &h();

        constexpr vector operator+(const vector &rhs) const;
        constexpr vector operator-(const vector &rhs) const;
        constexpr vector operator*(const float_t rhs) const;
        constexpr vector operator/(const float_t rhs) const;

        vector &operator=(const vector &rhs);
        vector &operator=(vector &&rhs);

        vector &operator+=(const vector &rhs);
        vector &operator-=(const vector &rhs);
        vector &operator*=(const float_t rhs);
        vector &operator/=(const float_t rhs);

        constexpr vector operator-() const { return (float_t)(-1) * *this; }

        constexpr bool operator==(const vector &that) const;
        constexpr bool operator!=(const vector &that) const { return !(*this == that); }

        void normalize();
        void divide_by_h();
        constexpr vector<float_t, dim + 1> homo() const;
        constexpr vector<float_t, dim - 1> dehomo() const;

        constexpr float_t length() const;
        constexpr float_t length_sqr() const;
        constexpr vector unit() const;
        constexpr float_t dot(const vector &rhs) const;
        constexpr vector cross(const vector &rhs) const;
        constexpr float_t angle(const vector &rhs) const;

    public: // Mandatory inlines
        friend constexpr vector operator*(float_t lhs, const vector& rhs) { return rhs * lhs; }

    public:
        template<typename, int> friend class vector;
        friend class matrix<float_t, dim, dim>;

    private:
        float_t elements[dim];
    };
};

#include "vector.cpp.h"
#include "vector_typedefs.h"

#endif
