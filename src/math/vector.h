#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H

#include <initializer_list>

namespace math {
    template<typename float_t, int dim> class vector;
    template<typename float_t, int rows, int columns> class matrix;
    
    template<typename float_t, int dim> class vector {
    public:
        vector();
        vector(const vector &rhs);
        vector(vector &&rhs);
        vector(const std::initializer_list<float_t> &il);

        float_t operator[](int i) const;
        float_t &operator[](int i);

        float_t x() const;
        float_t y() const;
        float_t z() const;
        float_t h() const;
        float_t &x();
        float_t &y();
        float_t &z();
        float_t &h();

        vector operator+(const vector &rhs) const;
        vector operator-(const vector &rhs) const;
        vector operator*(const float_t rhs) const;
        vector operator/(const float_t rhs) const;
        
        vector &operator=(const vector &rhs);
        vector &operator=(vector &&rhs);
		
        vector &operator+=(const vector &rhs);
        vector &operator-=(const vector &rhs);
        vector &operator*=(const float_t rhs);
        vector &operator/=(const float_t rhs);
	
        vector operator-() const { return (float_t)(-1) * *this; }
	
        bool operator==(const vector &that) const;
        bool operator!=(const vector &that) const { return !(*this == that); }

        void normalize();
        void divide_by_h();
        
        float_t length() const;
        vector unit() const;
        float_t dot(const vector &rhs) const;
        vector cross(const vector &rhs) const;
        float_t angle(const vector &rhs) const;
        
    public: // Mandatory inlines
        friend vector operator*(float_t lhs, const vector& rhs) { return rhs * lhs; }

    public:
        friend class matrix<float_t, dim, dim>;

    private:
        float_t elements[dim];
    };
};

#include "vector.cpp.h"
#include "vector_typedefs.h"

#endif
