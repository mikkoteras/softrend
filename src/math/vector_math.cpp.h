namespace math {
    template<typename float_t> vector<float_t, 2> normalize_projected(const vector<float_t, 4> &v) {
        return vector<float_t, 2>{v.x() / v.h(), v.y() / v.h()};
    }
}
