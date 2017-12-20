#include "test_harness.h"
#include "vector_tests.h"
#include "vector.h"
#include <cmath>

void vector_creation_tests();
void vector_comparison_tests();
void vector_arithmetic_tests();
void vector_assignment_tests();
void vector_geometry_tests();

using namespace math;

void vector_tests() {
    vector_creation_tests();
    vector_comparison_tests();
    vector_arithmetic_tests();
    vector_assignment_tests();
    vector_geometry_tests();
}

void vector_creation_tests() {
    vector2f t;
    ASSERT(t[0] == 0 && t[1] == 0);

    vector3d v { 1.0, 2.0, 3.0 };
    ASSERT(v[0] == 1 && v[1] == 2 && v[2] == 3);

    vector4d u { 1, 2, 3, 1 };
    ASSERT(u[0] == 1 && u[1] == 2 && u[2] == 3 && u[3] == 1);

    vector3d a(v);
    ASSERT(a[0] == v[0] && a[1] == v[1] && a[2] == v[2]);

    vector4d b = u;
    ASSERT(b[0] == u[0] && b[1] == u[1] && b[2] == u[2] && b[3] == u[3]);
    ASSERT(b[0] == u.x() && b[1] == u.y() && b[2] == u.z() && b[3] == u.h());

    vector3f x{1, 2, 3};
    vector3f y;
    y[0] = x[0];
    y[1] = x[1];
    y[2] = x[2];
    ASSERT(x[0] == y[0] && x[1] == y[1] && x[2] == y[2]);

    vector4f q{6, 7, 8, 9};
    vector4f r;
    r.x() = q[0];
    r.y() = q[1];
    r.z() = q[2];
    r.h() = q[3];
    ASSERT(q == r);
}

void vector_comparison_tests() {
    vector3d t{2, 3, 4};
    vector3d u{2, 3, 4};
    vector3d v = u;
    ASSERT(t == u && u == v && t == v);

    vector4d a{2, 3, 4, 1};
    vector4d b{2, 3, 4, 2};
    vector4d c(b);
    c[3] = 1;
    ASSERT(a != b);
    ASSERT(b != c);
    ASSERT(a == c);
}

void vector_arithmetic_tests() {
    vector4d a{3, 6, 9, 15};
    vector4d b{2, 3, 4, 1};
    vector4d c = -a;

    ASSERT((a + b == vector4d{5, 9, 13, 16}));
    ASSERT((b + a == vector4d{5, 9, 13, 16}));
    ASSERT((a - b == vector4d{1, 3, 5, 14}));
    ASSERT((b - a == vector4d{-1, -3, -5, -14}));
    ASSERT((2.0 * a == vector4d{6, 12, 18, 30}));
    ASSERT((a * 2.0 == vector4d{6, 12, 18, 30}));
    ASSERT((a / 3.0 == vector4d{1, 2, 3, 5}));
    ASSERT((a / 3.0 == vector4d{1, 2, 3, 5}));
    ASSERT((c == vector4d{-3, -6, -9, -15}));
    ASSERT((c == vector4d() - a));
}

void vector_assignment_tests() {
    vector3f a{3, 6, 9};
    vector3f b{1, 2, 3};

    vector3f c; c = a; c += b;
    ASSERT((c == vector3f{4, 8, 12}));

    vector3f d; d = a; d -= b;
    ASSERT((d == vector3f{2, 4, 6}));

    vector3f e; e = a; e *= 3;
    ASSERT((e == vector3f{9, 18, 27}));

    vector3f f; f = a; f /= 3;
    ASSERT((f == vector3f{1, 2, 3}));
}

void vector_geometry_tests() {
    vector3d a{1, 2, -2};
    ASSERT(a.length() == 3);

    vector3d b{2, 0, 0};
    ASSERT((b.unit() == vector3d{1, 0, 0}));

    vector3d c{0, -3, 0};
    ASSERT((c.unit() == vector3d{0, -1, 0}));

    vector3d d{0, 0, 4};
    ASSERT((d.unit() == vector3d{0, 0, 1}));

    vector3d e{2, 2, 0};
    vector3d f{2, -2, 0};
    vector3d g = e;
    ASSERT(e.dot(f) == 0);
    ASSERT(e.dot(g) == 8);

    vector3f x{2, 0, 0};
    vector3f y{0, 2, 0};
    vector3f z{0, 0, 2};
    ASSERT((x.cross(y) == vector3f{0, 0, 4}));
    ASSERT((y.cross(z) == vector3f{4, 0, 0}));
    ASSERT((z.cross(x) == vector3f{0, 4, 0}));

    vector4d p{9, 8, 7, 6};
    p.normalize();
    ASSERT((p.length() == 1));

    // Caution -- using doubles here is too accurate for comparison with ==.
    const float pi = M_PI;
    typedef vector<float, 5> vector5f;
    ASSERT((vector5f{10, 5, 6, 9, 2}.angle(vector5f{10, 5, 6, 9, 2}) / pi == 0.0));
    ASSERT((vector3f{10, 0, 0}.angle(vector3f{0, 0, 10}) / pi == 0.5f));
    ASSERT((vector2f{10, 0}.angle(vector2f{3, 3}) / pi == 0.25f));
    ASSERT((vector4f{10, 0, 0, 0}.angle(vector4f{-10, 10, 0, 0}) / pi == 0.75f));
}
