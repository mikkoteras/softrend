#include "line.h"
#include "color.h"
#include "mesh.h"
#include "scene.h"
#include "framebuffer.h"

using namespace math;

line::line() {
}

line::line(int vi1, int vi2, const color &c1, const color &c2) {
    vertex_index[0] = vi1;
    vertex_index[1] = vi2;
    endpoint_color[0] = c1;
    endpoint_color[1] = c2;
}

line::line(const line &rhs) {
    for (int i = 0; i < 2; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        endpoint_color[i] = rhs.endpoint_color[i];
    }
}

line::line(line &&rhs) {
    for (int i = 0; i < 2; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        endpoint_color[i] = rhs.endpoint_color[i];
    }
}

const line &line::operator=(const line &rhs) {
    for (int i = 0; i < 2; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        endpoint_color[i] = rhs.endpoint_color[i];
    }

    return *this;
}

line &line::operator=(line &&rhs) {
    for (int i = 0; i < 2; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        endpoint_color[i] = rhs.endpoint_color[i];
    }

    return *this;
}

line::~line() {
}

void line::render(framebuffer &target, const mesh &parent_mesh, const scene &parent_scene) const {
    const vector4f *view_coordinates = parent_mesh.view_coordinate_data();
    vector4f v1 = view_coordinates[vertex_index[0]];
    vector4f v2 = view_coordinates[vertex_index[1]];

    // plane clip
    if (parent_scene.visible_volume().clip(v1, v2))
        return;

    int width = fabs(v2.x() - v1.x());
    int height = fabs(v2.y() - v1.y());

    if (width == 0 && height == 0) {
        float z = std::min(v1.z(), v2.z());
        color c = (endpoint_color[0] + endpoint_color[1]) / 2.0f;
        target.set_pixel(v1.x(), v1.y(), z, c);
    }
    else if (width >= height) {
        float y, y_delta;
        int x, x_max;
        float z, z_delta;
        color c, c_delta;

        if (v1.x() < v2.x()) {
            x = v1.x();
            x_max = v2.x();
            y = v1.y();
            y_delta = (v2.y() - y) / width;
            z = v1.z();
            z_delta = (v2.z() - z) / width;
            c = endpoint_color[0];
            c_delta = (endpoint_color[1] - c) / width;
        }
        else {
            x = v2.x();
            x_max = v1.x();
            y = v2.y();
            y_delta = (v1.y() - y) / width;
            z = v2.z();
            z_delta = (v1.z() - z) / width;
            c = endpoint_color[1];
            c_delta = (endpoint_color[0] - c) / width;
        }

        if (x < 0) {
            y += -x * y_delta;
            z += -x * z_delta;
            c += -x * c_delta;
            x = 0;
        }

        if (x_max >= target.pixel_width())
            x_max = target.pixel_width() - 1;

        for (; x <= x_max; ++x) {
            if (z <= 0.0f)
                target.set_pixel(x, y, z, c);

            y += y_delta;
            z += z_delta;
            c += c_delta;
        }
    }
    else {
        float x, x_delta;
        int y, y_max;
        float z, z_delta;
        color c, c_delta;

        if (v1.y() < v2.y()) {
            y = v1.y();
            y_max = v2.y();
            x = v1.x();
            x_delta = (v2.x() - x) / height;
            z = v1.z();
            z_delta = (v2.z() - z) / height;
            c = endpoint_color[0];
            c_delta = (endpoint_color[1] - c) / height;
        }
        else {
            y = v2.y();
            y_max = v1.y();
            x = v2.x();
            x_delta = (v1.x() - x) / height;
            z = v2.z();
            z_delta = (v1.z() - z) / height;
            c = endpoint_color[1];
            c_delta = (endpoint_color[0] - c) / height;
        }

        if (y < 0) {
            x += -y * x_delta;
            z += -y * z_delta;
            c += -y * c_delta;
            y = 0;
        }

        if (y_max >= target.pixel_height())
            y_max = target.pixel_height() - 1;

        for (; y <= y_max; ++y) {
            if (z <= 0.0f)
                target.set_pixel(x, y, z, c);

            x += x_delta;
            z += z_delta;
            c += c_delta;
        }
    }
}

void line::render(framebuffer &target,
                  float x1, float y1, float z1, const color &c1,
                  float x2, float y2, float z2, const color &c2) {
    int width = abs(x2 - x1);
    int height = abs(y2 - y1);

    if (width == 0 && height == 0) {
        float z = std::min(z1, z2);
        color c = (c1 + c2) / 2.0f;
        target.set_pixel(x1, y1, z, c);
    }
    else if (width >= height) {
        float y, y_delta;
        int x, x_max;
        float z, z_delta;
        color c, c_delta;

        if (x1 < x2) {
            x = x1;
            x_max = x2;
            y = y1;
            y_delta = (y2 - y1) / width;
            z = z1;
            z_delta = (z2 - z1) / width;
            c = c1;
            c_delta = (c2 - c1) / width;
        }
        else {
            x = x2;
            x_max = x1;
            y = y2;
            y_delta = (y1 - y2) / width;
            z = z2;
            z_delta = (z1 - z2) / width;
            c = c2;
            c_delta = (c1 - c2) / width;
        }

        if (x < 0) {
            y += -x * y_delta;
            z += -x * z_delta;
            c += -x * c_delta;
            x = 0;
        }

        if (x_max >= target.pixel_width())
            x_max = target.pixel_width() - 1;

        for (; x <= x_max; ++x) {
            if (z <= 0.0f)
                target.set_pixel(x, y, z, c);

            y += y_delta;
            z += z_delta;
            c += c_delta;
        }
    }
    else {
        float x, x_delta;
        int y, y_max;
        float z, z_delta;
        color c, c_delta;

        if (y1 < y2) {
            y = y1;
            y_max = y2;
            x = x1;
            x_delta = (x2 - x1) / height;
            z = z1;
            z_delta = (z2 - z1) / height;
            c = c1;
            c_delta = (c2 - c1) / height;
        }
        else {
            y = y2;
            y_max = y1;
            x = x2;
            x_delta = (x1 - x2) / height;
            z = z2;
            z_delta = (z1 - z2) / height;
            c = c2;
            c_delta = (c1 - c2) / height;
        }

        if (y < 0) {
            x += -y * x_delta;
            z += -y * z_delta;
            c += -y * c_delta;
            y = 0;
        }

        if (y_max >= target.pixel_height())
            y_max = target.pixel_height() - 1;

        for (; y <= y_max; ++y) {
            if (z <= 0.0f)
                target.set_pixel(x, y, z, c);

            x += x_delta;
            z += z_delta;
            c += c_delta;
        }
    }
}
