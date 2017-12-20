#include "math_detail.h"
#include <cmath>

namespace math {
    template<typename float_t, int dim> constexpr vector<float_t, dim>::vector() {
        for (int i = 0; i < dim; ++i)
            elements[i] = (float_t)0;
    }

    template<typename float_t, int dim> vector<float_t, dim>::vector(const vector &rhs) {
        for (int i = 0; i < dim; ++i)
            elements[i] = rhs.elements[i];
    }

    template<typename float_t, int dim> vector<float_t, dim>::vector(vector &&rhs) {
        for (int i = 0; i < dim; ++i)
            elements[i] = rhs.elements[i];
    }

    template<typename float_t, int dim> constexpr vector<float_t, dim>::vector(const std::initializer_list<float_t> &il) {
        int i = 0;

        for (const auto e: il)
            elements[i++] = e;
    }

    template<typename float_t, int dim> constexpr vector<float_t, dim>::vector(const vector<float_t, dim - 1> &v, float_t h) {
        for (int i = 0; i < dim - 1; ++i)
            elements[i] = v.elements[i];

        elements[dim - 1] = h;
    }

    template<typename float_t, int dim> constexpr float_t vector<float_t, dim>::operator[](int i) const {
        return elements[i];
    }

    template<typename float_t, int dim> constexpr float_t &vector<float_t, dim>::operator[](int i) {
        return elements[i];
    }

    template<typename float_t, int dim> constexpr float_t vector<float_t, dim>::x() const {
        static_assert(dim >= 1, "vector::x() is undefined when dim < 1");
        return elements[0];
    }

    template<typename float_t, int dim> constexpr float_t vector<float_t, dim>::y() const {
        static_assert(dim >= 2, "vector::y() is undefined when dim < 2");
        return elements[1];
    }

    template<typename float_t, int dim> constexpr float_t vector<float_t, dim>::z() const {
        static_assert(dim >= 3, "vector::z() is undefined when dim < 3");
        return elements[2];
    }

    template<typename float_t, int dim> constexpr float_t vector<float_t, dim>::h() const {
        static_assert(dim >= 1, "vector::h() is undefined when dim < 1");
        return elements[dim - 1];
    }

    template<typename float_t, int dim> constexpr float_t &vector<float_t, dim>::x() {
        static_assert(dim >= 1, "vector::x() is undefined when dim < 1");
        return elements[0];
    }

    template<typename float_t, int dim> constexpr float_t &vector<float_t, dim>::y() {
        static_assert(dim >= 2, "vector::y() is undefined when dim < 2");
        return elements[1];
    }

    template<typename float_t, int dim> constexpr float_t &vector<float_t, dim>::z() {
        static_assert(dim >= 3, "vector::z() is undefined when dim < 3");
        return elements[2];
    }

    template<typename float_t, int dim> constexpr float_t &vector<float_t, dim>::h() {
        static_assert(dim >= 1, "vector::h() is undefined when dim < 1");
        return elements[dim - 1];
    }

    template<typename float_t, int dim> constexpr vector<float_t, dim> vector<float_t, dim>::operator+(const vector &rhs) const {
        vector<float_t, dim> result;

        for (int i = 0; i < dim; ++i)
            result.elements[i] = elements[i] + rhs.elements[i];

        return result;
    }

    template<typename float_t, int dim> constexpr vector<float_t, dim> vector<float_t, dim>::operator-(const vector &rhs) const {
        vector<float_t, dim> result;

        for (int i = 0; i < dim; ++i)
            result.elements[i] = elements[i] - rhs.elements[i];

        return result;
    }

    template<typename float_t, int dim> constexpr vector<float_t, dim> vector<float_t, dim>::operator*(const float_t rhs) const {
        vector<float_t, dim> result;

        for (int i = 0; i < dim; ++i)
            result.elements[i] = elements[i] * rhs;

        return result;
    }

    template<typename float_t, int dim> constexpr vector<float_t, dim> vector<float_t, dim>::operator/(const float_t rhs) const {
        vector<float_t, dim> result;

        for (int i = 0; i < dim; ++i)
            result.elements[i] = elements[i] / rhs;

        return result;
    }

    template<typename float_t, int dim> vector<float_t, dim> &vector<float_t, dim>::operator=(const vector &rhs) {
        for (int i = 0; i < dim; ++i)
            elements[i] = rhs.elements[i];

        return *this;
    }

    template<typename float_t, int dim> vector<float_t, dim> &vector<float_t, dim>::operator=(vector &&rhs) {
        for (int i = 0; i < dim; ++i)
            elements[i] = rhs.elements[i];

        return *this;
    }

    template<typename float_t, int dim> vector<float_t, dim> &vector<float_t, dim>::operator+=(const vector &rhs) {
        for (int i = 0; i < dim; ++i)
            elements[i] += rhs.elements[i];

        return *this;
    }

    template<typename float_t, int dim> vector<float_t, dim> &vector<float_t, dim>::operator-=(const vector &rhs) {
        for (int i = 0; i < dim; ++i)
            elements[i] -= rhs.elements[i];

        return *this;
    }

    template<typename float_t, int dim> vector<float_t, dim> &vector<float_t, dim>::operator*=(const float_t rhs) {
        for (int i = 0; i < dim; ++i)
            elements[i] *= rhs;

        return *this;
    }

    template<typename float_t, int dim> vector<float_t, dim> &vector<float_t, dim>::operator/=(const float_t rhs) {
        for (int i = 0; i < dim; ++i)
            elements[i] /= rhs;

        return *this;
    }

    template<typename float_t, int dim> constexpr bool vector<float_t, dim>::operator==(const vector &that) const {
        for (int i = 0; i < dim; ++i)
            if (elements[i] != that.elements[i])
                return false;

        return true;
    }

    template<typename float_t, int dim> void vector<float_t, dim>::normalize() {
        float_t l = length();

        for (int i = 0; i < dim; ++i)
            elements[i] /= l;
    }

    template<typename float_t, int dim>
    vector<float_t, dim + 1> constexpr vector<float_t, dim>::homo() const {
        vector<float_t, dim + 1> result;

        for (int i = 0; i < dim; ++i)
            result.elements[i] = elements[i];

        result.elements[dim] = 1;
        return result;
    }

    template<typename float_t, int dim>
    constexpr vector<float_t, dim - 1> vector<float_t, dim>::dehomo() const {
        vector<float_t, dim - 1> result;

        for (int i = 0; i < dim - 1; ++i)
            result.elements[i] = elements[i];

        return result;
    }

    template<typename float_t, int dim>
    constexpr vector<float_t, dim - 1> vector<float_t, dim>::dehomo_with_divide() const {
        vector<float_t, dim - 1> result;

        for (int i = 0; i < dim - 1; ++i)
            result.elements[i] = elements[i] / elements[dim - 1];

        return result;
    }

    template<typename float_t, int dim> constexpr float_t vector<float_t, dim>::length() const {
        float_t sqr_sum = 0;

        for (int i = 0; i < dim; ++i)
            sqr_sum = fma(elements[i], elements[i], sqr_sum);

        return (float_t)std::sqrt(sqr_sum);
    }

    template<typename float_t, int dim> constexpr float_t vector<float_t, dim>::length_sqr() const {
        float_t sqr_sum = 0;

        for (int i = 0; i < dim; ++i)
            sqr_sum = fma(elements[i], elements[i], sqr_sum);

        return sqr_sum;
    }

    template<typename float_t, int dim> constexpr vector<float_t, dim> vector<float_t, dim>::unit() const {
        return *this / length();
    }

    template<typename float_t, int dim> constexpr float_t vector<float_t, dim>::dot(const vector &rhs) const {
        float_t result = 0;

        for (int i = 0; i < dim; ++i)
            result = fma(elements[i], rhs.elements[i], result);

        return result;
    }

    template<typename float_t, int dim> constexpr vector<float_t, dim> vector<float_t, dim>::cross(const vector &rhs) const {
        static_assert(dim == 3, "vector::cross() is undefined when dim != 3");
        return vector<float_t, 3>{
            // TODO can this use FMA?
            elements[1] * rhs.elements[2] - elements[2] * rhs.elements[1],
            elements[2] * rhs.elements[0] - elements[0] * rhs.elements[2],
            elements[0] * rhs.elements[1] - elements[1] * rhs.elements[0]
        };
    }

    template<typename float_t, int dim> constexpr float_t vector<float_t, dim>::angle(const vector &rhs) const {
        return detail::acos<float_t>(dot(rhs) / (length() * rhs.length()));
    }
}
