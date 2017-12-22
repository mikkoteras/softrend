#include "triangle.h"
#include "color.h"
#include "framebuffer.h"
#include "light.h"
#include "light_list.h"
#include "linear_transforms.h"
#include "material.h"
#include "math_util.h"
#include "matrix.h"
#include "scene.h"
#include <algorithm>

using namespace math;

triangle::triangle() {
}

triangle::triangle(int vi1, int vi2, int vi3,
                   int ni1, int ni2, int ni3,
                   const math::vector3f &uv1, const math::vector3f &uv2, const math::vector3f &uv3,
                   const material *mat) :
    vertex_index{vi1, vi2, vi3},
    vertex_uv{uv1, uv2, uv3},
    normal_index{ni1, ni2, ni3},
    mat(mat),
    has_distinct_normals(ni1 != ni2 || ni1 != ni3),
    has_uv_coordinates(true),
    shading_limit(compute_shading_limit()) {
}

triangle::triangle(int vi1, int vi2, int vi3, int ni1, int ni2, int ni3, const material *mat) :
    vertex_index{vi1, vi2, vi3},
    vertex_uv{vector3f(), vector3f(), vector3f()},
    normal_index{ni1, ni2, ni3},
    mat(mat),
    has_distinct_normals(ni1 != ni2 || ni1 != ni3),
    has_uv_coordinates(false),
    shading_limit(compute_shading_limit()) {
}

triangle::triangle(const triangle &rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
        normal_index[i] = rhs.normal_index[i];
    }

    mat = rhs.mat;
    has_distinct_normals = rhs.has_distinct_normals;
    has_uv_coordinates = rhs.has_uv_coordinates;
    shading_limit = rhs.shading_limit;
}

triangle::triangle(triangle &&rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
        normal_index[i] = rhs.normal_index[i];
    }

    mat = rhs.mat;
    has_distinct_normals = rhs.has_distinct_normals;
    has_uv_coordinates = rhs.has_uv_coordinates;
    shading_limit = rhs.shading_limit;
}

const triangle &triangle::operator=(const triangle &rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
        normal_index[i] = rhs.normal_index[i];
    }

    mat = rhs.mat;
    has_distinct_normals = rhs.has_distinct_normals;
    has_uv_coordinates = rhs.has_uv_coordinates;
    shading_limit = rhs.shading_limit;
    return *this;
}

triangle &triangle::operator=(triangle &&rhs) {
    for (int i = 0; i < 3; ++i) {
        vertex_index[i] = rhs.vertex_index[i];
        vertex_uv[i] = rhs.vertex_uv[i];
        normal_index[i] = rhs.normal_index[i];
    }

    mat = rhs.mat;
    has_distinct_normals = rhs.has_distinct_normals;
    has_uv_coordinates = rhs.has_uv_coordinates;
    shading_limit = rhs.shading_limit;
    return *this;
}

triangle::~triangle() {
}

const int *triangle::vertex_indices() const {
    return vertex_index;
}

void triangle::render(framebuffer &target, const scene &parent_scene) const {
    const vector3f *view_coord = parent_scene.view_coordinate_data();

    for (int i = 0; i < 3; ++i)
        render_context.vtx(i).view_position = view_coord[vertex_index[i]];

    if (triangle_winds_clockwise())
        return;

    const vector3f *world_coord = parent_scene.world_coordinate_data();

    for (int i = 0; i < 3; ++i)
        render_context.vtx(i).world_position = world_coord[vertex_index[i]];

    // plane clip
    if (parent_scene.visible_volume().clip(render_context.vtx(0).view_position,
                                           render_context.vtx(1).view_position,
                                           render_context.vtx(2).view_position))
        return;

    render_context.lights = &parent_scene.light_sources();
    render_context.tex = mat->get_texture_map();
    shading_model_t shading = parent_scene.get_shading_model();

    if (shading == flat || shading_limit == flat)
        render_flat(target, parent_scene);
    else if (shading == gouraud || shading_limit == gouraud)
        render_gouraud(target, parent_scene);
    else
        render_phong(target, parent_scene);
}

void triangle::render_flat(framebuffer &target, const scene &parent_scene) const {
    const vector3f *world_normal = parent_scene.world_normal_data();
    render_context.surface_midpoint = vector3f{0.0f, 0.0f, 0.0f};
    
    for (int i = 0; i < 3; ++i) {
        vertex_data &vtx = render_context.vtx(i);
        render_context.surface_midpoint += vtx.world_position;
        vtx.uv[0] = vertex_uv[i][0];
        vtx.uv[1] = vertex_uv[i][1];
    }

    render_context.surface_midpoint /= 3.0f;
    
    if (has_distinct_normals)
        render_context.surface_normal = world_normal[normal_index[0]];
    else
        render_context.surface_normal = (world_normal[normal_index[0]] +
                                         world_normal[normal_index[1]] +
                                         world_normal[normal_index[2]]) / 3.0f;

    if (render_context.tex) {
        for (int i = 0; i < 3; ++i) {
            vertex_data &vtx = render_context.vtx(i);
            vtx.uv[0] = vertex_uv[i][0];
            vtx.uv[1] = vertex_uv[i][1];
        }

        render_context.prepare_edges();
        render_context.prepare_upper_halftriangle();
        render_textured_flat_halftriangle(target);
        render_context.prepare_lower_halftriangle();
        render_textured_flat_halftriangle(target);
    }
    else {
        render_context.prepare_edges();
        render_context.prepare_upper_halftriangle();
        render_colored_flat_halftriangle(target);
        render_context.prepare_lower_halftriangle();
        render_colored_flat_halftriangle(target);
    }
}

void triangle::render_gouraud(framebuffer &target, const scene &parent_scene) const {
    const vector3f *world_data = parent_scene.world_coordinate_data();
    const vector3f *world_normal = parent_scene.world_normal_data();
    
    for (int i = 0; i < 3; ++i) {
        vector3f vertex = world_data[vertex_index[i]];
        render_context.vtx(i).shade = mat->shade_phong(vertex,
                                                       world_normal[normal_index[i]],
                                                       (parent_scene.get_eye_position() - vertex).unit(),
                                                       parent_scene.light_sources());
    }

    if (render_context.tex) {
        for (int i = 0; i < 3; ++i) {
            vertex_data &vtx = render_context.vtx(i);
            vtx.uv[0] = vertex_uv[i][0];
            vtx.uv[1] = vertex_uv[i][1];
        }

        render_context.prepare_edges();
        render_context.prepare_upper_halftriangle();
        render_textured_gouraud_halftriangle(target);
        render_context.prepare_lower_halftriangle();
        render_textured_gouraud_halftriangle(target);
    }
    else {
        render_context.prepare_edges();
        render_context.prepare_upper_halftriangle();
        render_colored_gouraud_halftriangle(target);
        render_context.prepare_lower_halftriangle();
        render_colored_gouraud_halftriangle(target);
    }
}

void triangle::render_phong(framebuffer &target, const scene &parent_scene) const {
    render_context.eye = parent_scene.get_eye_position();

    if (has_distinct_normals)
        render_smooth_phong(target, parent_scene);
    else
        render_flat_phong(target, parent_scene);
}

void triangle::render_smooth_phong(framebuffer &target, const scene &parent_scene) const {
    const vector3f *world_normal = parent_scene.world_normal_data();
    
    for (int i = 0; i < 3; ++i) // TODO refactor
        render_context.vtx(i).normal = world_normal[normal_index[i]];

    if (render_context.tex && has_uv_coordinates) {
        for (int i = 0; i < 3; ++i) {
            vertex_data &vtx = render_context.vtx(i);
            vtx.uv[0] = vertex_uv[i][0];
            vtx.uv[1] = vertex_uv[i][1];
        }

        render_context.prepare_edges();
        render_context.prepare_upper_halftriangle();
        render_textured_smooth_phong_halftriangle(target);
        render_context.prepare_lower_halftriangle();
        render_textured_smooth_phong_halftriangle(target);
    }
    else {
        render_context.prepare_edges();
        render_context.prepare_upper_halftriangle();
        render_colored_smooth_phong_halftriangle(target);
        render_context.prepare_lower_halftriangle();
        render_colored_smooth_phong_halftriangle(target);
    }
}

void triangle::render_flat_phong(framebuffer &target, const scene &parent_scene) const {
    const vector3f *world_normal = parent_scene.world_normal_data();
    render_context.surface_normal = vector3f{0.0f, 0.0f, 0.0f};

    for (int i = 0; i < 3; ++i) // TODO refactor
        render_context.surface_normal += world_normal[normal_index[i]];

    if (render_context.tex && has_uv_coordinates) {
        for (int i = 0; i < 3; ++i) {
            vertex_data &vtx = render_context.vtx(i);
            vtx.uv[0] = vertex_uv[i][0];
            vtx.uv[1] = vertex_uv[i][1];
        }

        render_context.prepare_edges();
        render_context.prepare_upper_halftriangle();
        render_textured_flat_phong_halftriangle(target);
        render_context.prepare_lower_halftriangle();
        render_textured_flat_phong_halftriangle(target);
    }
    else {
        render_context.prepare_edges();
        render_context.prepare_upper_halftriangle();
        render_colored_flat_phong_halftriangle(target);
        render_context.prepare_lower_halftriangle();
        render_colored_flat_phong_halftriangle(target);
    }
}

bool triangle::triangle_winds_clockwise() {
    float x[3], y[3];
    
    for (int i = 0; i < 3; ++i) {
        const vector3f &v = render_context.vtx(i).view_position;
        x[i] = v.x();
        y[i] = v.y();
    }

    float sum = (x[1] - x[0]) * (y[1] + y[0]) +
                (x[2] - x[1]) * (y[2] + y[1]) +
                (x[0] - x[2]) * (y[0] + y[2]);
    
    return sum < 0.0f;
}

void triangle::render_colored_flat_halftriangle(framebuffer &target) const {
    if (render_context.halftriangle_height == 0)
        return;

    vertex_data left = *render_context.left_edge_top;
    vertex_data right = *render_context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);
    
    if (y < 0) {
        left.add_v(-y, *render_context.left_edge_delta);
        right.add_v(-y, *render_context.right_edge_delta);
        y = 0;
    }

    color shade(mat->shade_flat(render_context.surface_midpoint,
                                render_context.surface_normal,
                                *render_context.lights));
    
    for (; y <= max_y; ++y) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        vertex_data pixel = left;
        vertex_data delta;
        delta.compute_delta_v(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_v(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            target.set_pixel_unchecked(x, y, pixel.view_position.z(), shade);
            pixel.add_v(delta); // TODO: skip view_position, use z alone
        }

        left.add_v(*render_context.left_edge_delta);
        right.add_v(*render_context.right_edge_delta);
    }
}

void triangle::render_colored_gouraud_halftriangle(framebuffer &target) const {
    if (render_context.halftriangle_height == 0)
        return;

    vertex_data left = *render_context.left_edge_top;
    vertex_data right = *render_context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);
    
    if (y < 0) {
        left.add_vs(-y, *render_context.left_edge_delta);
        right.add_vs(-y, *render_context.right_edge_delta);
        y = 0;
    }

    for (; y <= max_y; ++y) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        vertex_data pixel = left;
        vertex_data delta;
        delta.compute_delta_vs(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_vs(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            target.set_pixel_unchecked(x, y, pixel.view_position.z(), pixel.shade);
            pixel.add_vs(delta); // TODO: skip view_position, use z alone
        }

        left.add_vs(*render_context.left_edge_delta);
        right.add_vs(*render_context.right_edge_delta);
    }
}

void triangle::render_colored_smooth_phong_halftriangle(framebuffer &target) const {
    if (render_context.halftriangle_height == 0)
        return;

    vertex_data left = *render_context.left_edge_top;
    vertex_data right = *render_context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);

    if (y < 0) {
        left.add_vwn(-y, *render_context.left_edge_delta);
        right.add_vwn(-y, *render_context.right_edge_delta);
        y = 0;
    }

    // TODO: z-coord clip -- really necessary?

    for (; y <= max_y; ++y) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        vertex_data pixel = left;
        vertex_data delta;
        delta.compute_delta_vwn(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_vwn(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            color shade(mat->shade_phong(pixel.world_position,
                                         pixel.normal.unit(),
                                         (render_context.eye - pixel.world_position).unit(),
                                         *render_context.lights));
            target.set_pixel_unchecked(x, y, pixel.view_position.z(), shade);
            pixel.add_vwn(delta); // TODO: skip view_position, use z alone
        }

        left.add_vwn(*render_context.left_edge_delta);
        right.add_vwn(*render_context.right_edge_delta);
    }
}

void triangle::render_colored_flat_phong_halftriangle(framebuffer &target) const {
    if (render_context.halftriangle_height == 0)
        return;

    vertex_data left = *render_context.left_edge_top;
    vertex_data right = *render_context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);

    if (y < 0) {
        left.add_vw(-y, *render_context.left_edge_delta);
        right.add_vw(-y, *render_context.right_edge_delta);
        y = 0;
    }

    // TODO: z-coord clip -- really necessary?

    for (; y <= max_y; ++y) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        vertex_data pixel = left;
        vertex_data delta;
        delta.compute_delta_vw(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_vw(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            color shade(mat->shade_phong(pixel.world_position,
                                         render_context.surface_normal,
                                         (render_context.eye - pixel.world_position).unit(),
                                         *render_context.lights));
            target.set_pixel_unchecked(x, y, pixel.view_position.z(), shade);
            pixel.add_vw(delta); // TODO: skip view_position, use z alone
        }

        left.add_vw(*render_context.left_edge_delta);
        right.add_vw(*render_context.right_edge_delta);
    }
}

void triangle::render_textured_flat_halftriangle(framebuffer &target) const {
    if (render_context.halftriangle_height == 0)
        return;

    vertex_data left = *render_context.left_edge_top;
    vertex_data right = *render_context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);
    
    if (y < 0) {
        left.add_vt(-y, *render_context.left_edge_delta);
        right.add_vt(-y, *render_context.right_edge_delta);
        y = 0;
    }

    color shade(mat->shade_flat(render_context.surface_midpoint,
                                render_context.surface_normal,
                                *render_context.lights));
    
    for (; y <= max_y; ++y) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        vertex_data pixel = left;
        vertex_data delta;
        delta.compute_delta_vt(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_vt(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            target.set_pixel_unchecked(x, y, pixel.view_position.z(),
                                       shade * render_context.tex->at(pixel.uv[0], pixel.uv[1]));
            pixel.add_vt(delta); // TODO: skip view_position, use z alone
        }

        left.add_vt(*render_context.left_edge_delta);
        right.add_vt(*render_context.right_edge_delta);
    }
}

void triangle::render_textured_gouraud_halftriangle(framebuffer &target) const {
    if (render_context.halftriangle_height == 0)
        return;

    vertex_data left = *render_context.left_edge_top;
    vertex_data right = *render_context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);
    
    if (y < 0) {
        left.add_vts(-y, *render_context.left_edge_delta);
        right.add_vts(-y, *render_context.right_edge_delta);
        y = 0;
    }

    for (; y <= max_y; ++y) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        vertex_data pixel = left;
        vertex_data delta;
        delta.compute_delta_vts(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_vts(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            target.set_pixel_unchecked(x, y, pixel.view_position.z(),
                             pixel.shade * render_context.tex->at(pixel.uv[0], pixel.uv[1]));
            pixel.add_vts(delta); // TODO: skip view_position, use z alone
        }

        left.add_vts(*render_context.left_edge_delta);
        right.add_vts(*render_context.right_edge_delta);
    }
}

void triangle::render_textured_smooth_phong_halftriangle(framebuffer &target) const {
    if (render_context.halftriangle_height == 0)
        return;

    vertex_data left = *render_context.left_edge_top;
    vertex_data right = *render_context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);

    if (y < 0) {
        left.add_vwnt(-y, *render_context.left_edge_delta);
        right.add_vwnt(-y, *render_context.right_edge_delta);
        y = 0;
    }

    // TODO: z-coord clip -- really necessary?

    for (; y <= max_y; ++y) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        vertex_data pixel = left;
        vertex_data delta;
        delta.compute_delta_vwnt(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_vwnt(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            color shade(render_context.tex->at(pixel.uv[0], pixel.uv[1]));
            shade *= mat->shade_phong(pixel.world_position,
                                      pixel.normal.unit(),
                                      (render_context.eye - pixel.world_position).unit(),
                                      *render_context.lights);

            target.set_pixel_unchecked(x, y, pixel.view_position.z(), shade);
            pixel.add_vwnt(delta); // TODO: skip view_position, use z alone
        }

        left.add_vwnt(*render_context.left_edge_delta);
        right.add_vwnt(*render_context.right_edge_delta);
    }
}

void triangle::render_textured_flat_phong_halftriangle(framebuffer &target) const {
    if (render_context.halftriangle_height == 0)
        return;

    vertex_data left = *render_context.left_edge_top;
    vertex_data right = *render_context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + render_context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);

    if (y < 0) {
        left.add_vwt(-y, *render_context.left_edge_delta);
        right.add_vwt(-y, *render_context.right_edge_delta);
        y = 0;
    }

    // TODO: z-coord clip -- really necessary?

    for (; y <= max_y; ++y) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        vertex_data pixel = left;
        vertex_data delta;
        delta.compute_delta_vwt(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_vwt(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            color shade(render_context.tex->at(pixel.uv[0], pixel.uv[1]));
            shade *= mat->shade_phong(pixel.world_position,
                                      render_context.surface_normal,
                                      (render_context.eye - pixel.world_position).unit(),
                                      *render_context.lights);
            target.set_pixel_unchecked(x, y, pixel.view_position.z(), shade);
            pixel.add_vwt(delta); // TODO: skip view_position, use z alone
        }

        left.add_vwt(*render_context.left_edge_delta);
        right.add_vwt(*render_context.right_edge_delta);
    }
}

void triangle::visualize_normals(framebuffer &target, scene &parent_scene) const {
    const vector3f *view_data = parent_scene.view_coordinate_data();
    const vector3f *world_data = parent_scene.world_coordinate_data();
    const vector3f *normal_data = parent_scene.world_normal_data();
    const matrix4x4f &world_to_view = parent_scene.world_to_view();
    color yellow(1.0f, 1.0f, 0.0f, 1.0f);
    vector3f mid_point, mid_normal;

    for (int i = 0; i < 3; ++i) {
        int vi = vertex_index[i], ni = normal_index[i];

        vector3f vertex(view_data[vi]); // view vertex
        vector3f world_normal(world_data[vi] + 0.3f * normal_data[ni]); // world normal, offset from vertex
        vector3f view_normal((world_to_view * world_normal.homo()).dehomo_with_divide());
        line::render(target, vertex.x(), vertex.y(), vertex.z(), yellow,
                     view_normal.x(), view_normal.y(), view_normal.z(), yellow);

        mid_point += (world_data[vi] - mid_point) / (i + 1);
        mid_normal += (normal_data[ni] - mid_normal) / (i + 1);
    }

    mid_normal.normalize();
    vector3f world_normal(mid_point + 0.3f * mid_normal); // world normal, offset from midpoint
    vector3f view_point((world_to_view * mid_point.homo()).dehomo_with_divide());
    vector3f view_normal((world_to_view * world_normal.homo()).dehomo_with_divide());

    line::render(target, view_point.x(), view_point.y(), view_point.z(), yellow,
                 view_normal.x(), view_normal.y(), view_normal.z(), yellow);
}

void triangle::visualize_reflection_vectors(framebuffer &target, scene &parent_scene) const {
    const vector3f *world_data = parent_scene.world_coordinate_data();
    const vector3f *normal_data = parent_scene.world_normal_data();
    const matrix4x4f &world_to_view = parent_scene.world_to_view();
    
    // triangle midpoint and the normal out of it
    vector3f mid_point, mid_normal;

    for (int i = 0; i < 3; ++i) {
        mid_point += (world_data[vertex_index[i]] - mid_point) / (i + 1);
        mid_normal += (normal_data[normal_index[i]] - mid_normal) / (i + 1);
    }

    mid_normal.normalize();
    vector3f midpoint_to_eye = (parent_scene.get_eye_position() - mid_point).unit();

    // phong vectors for each light
    for (const light *source: parent_scene.light_sources().get()) {
        vector3f light_vector(source->surface_to_light_unit(mid_point));
        float normal_dot_light(mid_normal.dot(light_vector));

        vector3f reflection_vector(2.0f * normal_dot_light * mid_normal - light_vector);
        vector3f p = (world_to_view * mid_point.homo()).dehomo_with_divide();
        vector3f l = (world_to_view * (mid_point + 0.3f * light_vector).homo()).dehomo_with_divide();
        vector3f r = (world_to_view * (mid_point + 0.3f * reflection_vector).homo()).dehomo_with_divide();

        color light_color(source->specular());
        color reflected_color(light_color * mat->get_specular_reflectivity());

        line::render(target, p.x(), p.y(), p.z(), light_color, l.x(), l.y(), l.z(), light_color);

        if (midpoint_to_eye.dot(reflection_vector) > 0.0f)
            line::render(target, p.x(), p.y(), p.z(), reflected_color, r.x(), r.y(), r.z(), reflected_color);
    }
}

shading_model_t triangle::compute_shading_limit() {
    illumination_model_t illum = mat->get_illumination_model();

    if (illum == constant_color)
        return flat;
    else
        return phong;
}

triangle_render triangle::render_context;
