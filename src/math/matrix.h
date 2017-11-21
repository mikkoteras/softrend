#ifndef MATH_MATRIX_H
#define MATH_MATRIX_H

#include <initializer_list>
#include "vector.h"

namespace math {
    template<typename float_t, int rows, int columns> class matrix {
    public:
        typedef vector<float_t, columns> column_vector_t;
        const int size = rows * columns;
        template<typename, int, int> friend class matrix;
        template<typename, int> friend class vector;

    public:
        matrix();
        matrix(const matrix &rhs);
        matrix(matrix &&rhs);
        matrix(const std::initializer_list<float_t> &il);
        ~matrix();

        static matrix zero();
        static matrix identity();

        float_t at(int col, int row) const;
        float_t &at(int col, int row);

        matrix operator+(const matrix &rhs) const;
        matrix operator-(const matrix &rhs) const;
        template<int rhs_columns> matrix<float_t, rows, rhs_columns> operator*(const matrix<float_t, columns, rhs_columns> &rhs) const;
        column_vector_t operator*(const column_vector_t &rhs) const;

        matrix &operator=(const matrix &rhs);
        matrix &operator=(matrix &&rhs);

        matrix &operator+=(const matrix &rhs);
        matrix &operator-=(const matrix &rhs);
        matrix &operator*=(const matrix &rhs);

        bool operator==(const matrix &that) const;
        bool operator!=(const matrix &that) const { return !(*this == that); }

    private:
        float_t *elements;
    };
}

#include "matrix.cpp.h"
#include "matrix_typedefs.h"

#endif
