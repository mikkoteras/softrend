namespace math {
    template<typename float_t, int rows, int columns> matrix<float_t, rows, columns>::matrix() :
        elements(new float_t[size]) {
    }

    template<typename float_t, int rows, int columns> matrix<float_t, rows, columns>::matrix(const matrix &rhs) :
        matrix() {

        for (int i = 0; i < size; ++i)
            elements[i] = rhs.elements[i];
    }

    template<typename float_t, int rows, int columns> matrix<float_t, rows, columns>::matrix(matrix &&rhs) {
        elements = rhs.elements;
        rhs.elements = nullptr;
    }

    template<typename float_t, int rows, int columns> matrix<float_t, rows, columns>::matrix(const std::initializer_list<float_t> &il) :
        matrix() {

        int i = 0;

        for (const auto e: il)
            elements[i++] = e;
    }

    template<typename float_t, int rows, int columns> matrix<float_t, rows, columns>::~matrix() {
        delete[] elements;
    }

    template<typename float_t, int rows, int columns> matrix<float_t, rows, columns> matrix<float_t, rows, columns>::zero() {
        matrix result;

        for (int i = 0; i < result.size; ++i)
            result.elements[i] = (float_t)0;

        return result;
    }

    template<typename float_t, int rows, int columns> matrix<float_t, rows, columns> matrix<float_t, rows, columns>::identity() {
        static_assert(rows == columns, "matrix::identity() is only defined for square matrices");

        matrix result = zero();

        for (int i = 0; i < result.size; i += columns + 1)
            result.elements[i] = 1;

        return result;
    }

    template<typename float_t, int rows, int columns> float_t matrix<float_t, rows, columns>::at(int col, int row) const {
        return elements[row * columns + col];
    }

    template<typename float_t, int rows, int columns> float_t &matrix<float_t, rows, columns>::at(int col, int row) {
        return elements[row * columns + col];
    }

    template<typename float_t, int rows, int columns>
    matrix<float_t, rows, columns> matrix<float_t, rows, columns>::operator+(const matrix<float_t, rows, columns> &rhs) const {
        matrix result;

        for (int i = 0; i < size; ++i)
            result.elements[i] = elements[i] + rhs.elements[i];

        return result;
    }

    template<typename float_t, int rows, int columns>
    matrix<float_t, rows, columns> matrix<float_t, rows, columns>::operator-(const matrix<float_t, rows, columns> &rhs) const {
        matrix result;

        for (int i = 0; i < size; ++i)
            result.elements[i] = elements[i] - rhs.elements[i];

        return result;
    }

    template<typename float_t, int rows, int columns>
    template<int rhs_columns>
    matrix<float_t, rows, rhs_columns> matrix<float_t, rows, columns>::operator*(const matrix<float_t, columns, rhs_columns> &rhs) const {
        matrix<float_t, rows, rhs_columns> result;

        for (int row = 0, element_index = 0; element_index < result.size; ++row)
            for (int column = 0; column < rhs_columns; ++column, ++element_index) {
                float_t e = 0;
                int lhs_element_index = row * columns;
                int rhs_element_index = column;

                for (int i = 0; i < columns; ++i) {
                    e += elements[lhs_element_index] * rhs.elements[rhs_element_index];
                    lhs_element_index += 1;
                    rhs_element_index += rhs_columns;
                }

                result.elements[element_index] = e;
            }

        return result;
    }

    template<typename float_t, int rows, int columns>
        typename matrix<float_t, rows, columns>::column_vector_t
        matrix<float_t, rows, columns>::operator*(const column_vector_t &rhs) const {

        column_vector_t result;
        int matrix_element_index = 0;

        for (int row = 0; row < rows; ++row) {
            float_t e = 0;

            for (int column = 0; column < columns; ++column, ++matrix_element_index)
                e += elements[matrix_element_index] * rhs.elements[column];

            result.elements[row] = e;
        }

        return result;
    }

    template<typename float_t, int rows, int columns>
    matrix<float_t, rows, columns> &matrix<float_t, rows, columns>::operator=(const matrix<float_t, rows, columns> &rhs)
    {
        for (int i = 0; i < size; ++i)
            elements[i] = rhs.elements[i];

        return *this;
    }

    template<typename float_t, int rows, int columns>
    matrix<float_t, rows, columns> &matrix<float_t, rows, columns>::operator=(matrix<float_t, rows, columns> &&rhs)
    {
        if (this != &rhs) {
            delete[] elements;
            elements = rhs.elements;
            rhs.elements = nullptr;
        }

        return *this;
    }

    template<typename float_t, int rows, int columns> bool matrix<float_t, rows, columns>::operator==(const matrix<float_t, rows, columns> &that) const {
        for (int i = 0; i < size; ++i)
            if (elements[i] != that.elements[i])
                return false;

        return true;
    }
}
