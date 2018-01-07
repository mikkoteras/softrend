#include "triangle.h"
#include "color3.h"
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
                   const math::vector2f &uv1, const math::vector2f &uv2, const math::vector2f &uv3,
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
    vertex_uv{vector2f(), vector2f(), vector2f()},
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

bool triangle::has_transparency() const {
    return mat->has_transparency();
}

void triangle::render(framebuffer &target, const scene &parent_scene, triangle_render_context &context) const {
    const vector3f *view_coord = parent_scene.view_coordinate_data();

    for (int i = 0; i < 3; ++i)
        context.vtx(i).view_position = view_coord[vertex_index[i]];

    if (triangle_winds_clockwise(context))
        return;

    const vector3f *world_coord = parent_scene.world_coordinate_data();

    for (int i = 0; i < 3; ++i)
        context.vtx(i).world_position = world_coord[vertex_index[i]];

    // plane clip
    if (parent_scene.visible_volume().clip(context.vtx(0).view_position,
                                           context.vtx(1).view_position,
                                           context.vtx(2).view_position))
        return;

    context.lights = &parent_scene.light_sources();
    shading_model_t shading = parent_scene.get_shading_model();

    if (shading == flat || shading_limit == flat)
        render_flat(target, parent_scene, context);
    else if (shading == gouraud || shading_limit == gouraud)
        render_gouraud(target, parent_scene, context);
    else
        render_phong(target, parent_scene, context);
}

void triangle::render_flat(framebuffer &target, const scene &parent_scene, triangle_render_context &context) const {
    const vector3f *world_normal = parent_scene.world_normal_data();
    context.surface_midpoint = vector3f{0.0f, 0.0f, 0.0f};
    
    for (int i = 0; i < 3; ++i) {
        surface_position &vtx = context.vtx(i);
        context.surface_midpoint += vtx.world_position;
        vtx.uv[0] = vertex_uv[i][0];
        vtx.uv[1] = vertex_uv[i][1];
    }

    context.surface_midpoint /= 3.0f;
    
    if (has_distinct_normals)
        context.surface_normal = world_normal[normal_index[0]];
    else
        context.surface_normal = (world_normal[normal_index[0]] +
                                         world_normal[normal_index[1]] +
                                         world_normal[normal_index[2]]) / 3.0f;

    if (mat->is_textured()) {
        for (int i = 0; i < 3; ++i) {
            surface_position &vtx = context.vtx(i);
            vtx.uv[0] = vertex_uv[i][0];
            vtx.uv[1] = vertex_uv[i][1];
        }

        context.prepare_edges();
        context.prepare_upper_halftriangle();
        render_textured_flat_halftriangle(target, context);
        context.prepare_lower_halftriangle();
        render_textured_flat_halftriangle(target, context);
    }
    else {
        context.prepare_edges();
        context.prepare_upper_halftriangle();
        render_colored_flat_halftriangle(target, context);
        context.prepare_lower_halftriangle();
        render_colored_flat_halftriangle(target, context);
    }
}

void triangle::render_gouraud(framebuffer &target, const scene &parent_scene, triangle_render_context &context) const {
    const vector3f *world_data = parent_scene.world_coordinate_data();
    const vector3f *world_normal = parent_scene.world_normal_data();

    for (int i = 0; i < 3; ++i) {
        vector3f vertex = world_data[vertex_index[i]];
        context.vtx(i).shade = mat->shade(vertex,
                                                 world_normal[normal_index[i]],
                                                 (parent_scene.get_eye_position() - vertex).unit(),
                                                 vertex_uv[i], // FIXME -- add shade_gouraud()
                                                 parent_scene.light_sources());
    }

    if (mat->is_textured()) {
        for (int i = 0; i < 3; ++i) {
            surface_position &vtx = context.vtx(i);
            vtx.uv[0] = vertex_uv[i][0];
            vtx.uv[1] = vertex_uv[i][1];
        }

        context.prepare_edges();
        context.prepare_upper_halftriangle();
        render_textured_gouraud_halftriangle(target, context);
        context.prepare_lower_halftriangle();
        render_textured_gouraud_halftriangle(target, context);
    }
    else {
        context.prepare_edges();
        context.prepare_upper_halftriangle();
        render_colored_gouraud_halftriangle(target, context);
        context.prepare_lower_halftriangle();
        render_colored_gouraud_halftriangle(target, context);
    }
}

void triangle::render_phong(framebuffer &target, const scene &parent_scene,
                            triangle_render_context &context) const {
    context.eye = parent_scene.get_eye_position();

    if (has_distinct_normals)
        render_smooth_phong(target, parent_scene, context);
    else
        render_flat_phong(target, parent_scene, context);
}

void triangle::render_smooth_phong(framebuffer &target, const scene &parent_scene,
                                   triangle_render_context &context) const {
    const vector3f *world_normal = parent_scene.world_normal_data();
    
    for (int i = 0; i < 3; ++i) // TODO refactor
        context.vtx(i).normal = world_normal[normal_index[i]];

    if (mat->is_textured() && has_uv_coordinates) {
        for (int i = 0; i < 3; ++i) {
            surface_position &vtx = context.vtx(i);
            vtx.uv[0] = vertex_uv[i][0];
            vtx.uv[1] = vertex_uv[i][1];
        }

        context.prepare_edges();
        context.prepare_upper_halftriangle();
        render_textured_smooth_phong_halftriangle(target, context);
        context.prepare_lower_halftriangle();
        render_textured_smooth_phong_halftriangle(target, context);
    }
    else {
        context.prepare_edges();
        context.prepare_upper_halftriangle();
        render_colored_smooth_phong_halftriangle(target, context);
        context.prepare_lower_halftriangle();
        render_colored_smooth_phong_halftriangle(target, context);
    }
}

void triangle::render_flat_phong(framebuffer &target, const scene &parent_scene,
                                 triangle_render_context &context) const {
    const vector3f *world_normal = parent_scene.world_normal_data();

    if (has_distinct_normals)
        context.surface_normal = world_normal[normal_index[0]];
    else
        context.surface_normal = (world_normal[normal_index[0]] +
                                         world_normal[normal_index[1]] +
                                         world_normal[normal_index[2]]) / 3.0f;

    if (mat->is_textured() && has_uv_coordinates) {
        for (int i = 0; i < 3; ++i) {
            surface_position &vtx = context.vtx(i);
            vtx.uv[0] = vertex_uv[i][0];
            vtx.uv[1] = vertex_uv[i][1];
        }

        context.prepare_edges();
        context.prepare_upper_halftriangle();
        render_textured_flat_phong_halftriangle(target, context);
        context.prepare_lower_halftriangle();
        render_textured_flat_phong_halftriangle(target, context);
    }
    else {
        context.prepare_edges();
        context.prepare_upper_halftriangle();
        render_colored_flat_phong_halftriangle(target, context);
        context.prepare_lower_halftriangle();
        render_colored_flat_phong_halftriangle(target, context);
    }
}

bool triangle::triangle_winds_clockwise(triangle_render_context &context) {
    float x[3], y[3];
    
    for (int i = 0; i < 3; ++i) {
        const vector3f &v = context.vtx(i).view_position;
        x[i] = v.x();
        y[i] = v.y();
    }

    float sum = (x[1] - x[0]) * (y[1] + y[0]) +
                (x[2] - x[1]) * (y[2] + y[1]) +
                (x[0] - x[2]) * (y[0] + y[2]);
    
    return sum < 0.0f;
}

void triangle::render_colored_flat_halftriangle(framebuffer &target, triangle_render_context &context) const {
    if (context.halftriangle_height <= 0)
        return;

    surface_position left = *context.left_edge_top;
    surface_position right = *context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);

    if (y < 0) {
        left.add_v(-y, *context.left_edge_delta);
        right.add_v(-y, *context.right_edge_delta);
        y = 0;
    }

    color4 shade(mat->shade(context.surface_midpoint,
                            context.surface_normal,
                            (context.eye - context.surface_midpoint).unit(),
                            *context.lights));

    for (; y <= max_y; ++y) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta_v(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_v(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            target.set_pixel_with_z_clip(x, y, pixel.view_position.z(), shade);
            pixel.add_v(delta); // TODO: skip view_position, use z alone
        }

        left.add_v(*context.left_edge_delta);
        right.add_v(*context.right_edge_delta);
    }
}

void triangle::render_colored_gouraud_halftriangle(framebuffer &target, triangle_render_context &context) const {
    if (context.halftriangle_height <= 0)
        return;

    surface_position left = *context.left_edge_top;
    surface_position right = *context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);

    if (y < 0) {
        left.add_vs(-y, *context.left_edge_delta);
        right.add_vs(-y, *context.right_edge_delta);
        y = 0;
    }
    
    for (; y <= max_y; ++y) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta_vs(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_vs(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            target.set_pixel_with_z_clip(x, y, pixel.view_position.z(), pixel.shade);
            pixel.add_vs(delta); // TODO: skip view_position, use z alone
        }

        left.add_vs(*context.left_edge_delta);
        right.add_vs(*context.right_edge_delta);
    }
}

void triangle::render_colored_smooth_phong_halftriangle(framebuffer &target, triangle_render_context &context) const {
    if (context.halftriangle_height <= 0)
        return;

    surface_position left = *context.left_edge_top;
    surface_position right = *context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);

    if (y < 0) {
        left.add_vwn(-y, *context.left_edge_delta);
        right.add_vwn(-y, *context.right_edge_delta);
        y = 0;
    }

    // TODO: z-coord clip -- really necessary?

    for (; y <= max_y; ++y) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta_vwn(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_vwn(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            if (target.depth_at(x, y) < pixel.view_position.z())
                target.set_pixel_overwriting_z_buffer(x, y, pixel.view_position.z(),
                                                      mat->shade(pixel.world_position,
                                                                 pixel.normal.unit(),
                                                                 (context.eye - pixel.world_position).unit(),
                                                                 *context.lights));
            
            pixel.add_vwn(delta); // TODO: skip view_position, use z alone
        }

        left.add_vwn(*context.left_edge_delta);
        right.add_vwn(*context.right_edge_delta);
    }
}

void triangle::render_colored_flat_phong_halftriangle(framebuffer &target, triangle_render_context &context) const {
    if (context.halftriangle_height <= 0)
        return;

    surface_position left = *context.left_edge_top;
    surface_position right = *context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);

    if (y < 0) {
        left.add_vw(-y, *context.left_edge_delta);
        right.add_vw(-y, *context.right_edge_delta);
        y = 0;
    }

    // TODO: z-coord clip -- really necessary?

    for (; y <= max_y; ++y) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta_vw(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_vw(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            if (target.depth_at(x, y) < pixel.view_position.z())
                target.set_pixel_overwriting_z_buffer(x, y, pixel.view_position.z(),
                                                      mat->shade(pixel.world_position,
                                                                 context.surface_normal,
                                                                 (context.eye - pixel.world_position).unit(),
                                                                 pixel.uv,
                                                                 *context.lights));

            pixel.add_vw(delta); // TODO: skip view_position, use z alone
        }

        left.add_vw(*context.left_edge_delta);
        right.add_vw(*context.right_edge_delta);
    }
}

void triangle::render_textured_flat_halftriangle(framebuffer &target, triangle_render_context &context) const {
    if (context.halftriangle_height <= 0)
        return;

    surface_position left = *context.left_edge_top;
    surface_position right = *context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);
    
    if (y < 0) {
        left.add_vt(-y, *context.left_edge_delta);
        right.add_vt(-y, *context.right_edge_delta);
        y = 0;
    }

    color4 shade(mat->shade(context.surface_midpoint,
                            context.surface_normal,
                            (context.eye - context.surface_midpoint).unit(),
                            *context.lights));

    for (; y <= max_y; ++y) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta_vt(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_vt(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            if (target.depth_at(x, y) < pixel.view_position.z())
                target.set_pixel_overwriting_z_buffer(x, y, pixel.view_position.z(),
                                                      shade * mat->diffuse_texture_map(pixel.uv));
            
            pixel.add_vt(delta); // TODO: skip view_position, use z alone
        }

        left.add_vt(*context.left_edge_delta);
        right.add_vt(*context.right_edge_delta);
    }
}

void triangle::render_textured_gouraud_halftriangle(framebuffer &target, triangle_render_context &context) const {
    if (context.halftriangle_height <= 0)
        return;

    surface_position left = *context.left_edge_top;
    surface_position right = *context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);
    
    if (y < 0) {
        left.add_vts(-y, *context.left_edge_delta);
        right.add_vts(-y, *context.right_edge_delta);
        y = 0;
    }

    for (; y <= max_y; ++y) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta_vts(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_vts(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            if (target.depth_at(x, y) < pixel.view_position.z())
                target.set_pixel_overwriting_z_buffer(x, y, pixel.view_position.z(),
                                                      pixel.shade * mat->diffuse_texture_map(pixel.uv));
            
            pixel.add_vts(delta); // TODO: skip view_position, use z alone
        }

        left.add_vts(*context.left_edge_delta);
        right.add_vts(*context.right_edge_delta);
    }
}

void triangle::render_textured_smooth_phong_halftriangle(framebuffer &target, triangle_render_context &context) const {
    if (context.halftriangle_height <= 0)
        return;

    surface_position left = *context.left_edge_top;
    surface_position right = *context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);

    if (y < 0) {
        left.add_vwnt(-y, *context.left_edge_delta);
        right.add_vwnt(-y, *context.right_edge_delta);
        y = 0;
    }

    // TODO: z-coord clip -- really necessary?

    for (; y <= max_y; ++y) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta_vwnt(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_vwnt(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            if (target.depth_at(x, y) < pixel.view_position.z())
                target.set_pixel_overwriting_z_buffer(x, y, pixel.view_position.z(),
                                                      mat->shade(pixel.world_position,
                                                                 pixel.normal.unit(),
                                                                 (context.eye - pixel.world_position).unit(),
                                                                 pixel.uv,
                                                                 *context.lights));

            pixel.add_vwnt(delta); // TODO: skip view_position, use z alone
        }

        left.add_vwnt(*context.left_edge_delta);
        right.add_vwnt(*context.right_edge_delta);
    }
}

void triangle::render_textured_flat_phong_halftriangle(framebuffer &target, triangle_render_context &context) const {
    if (context.halftriangle_height <= 0)
        return;

    surface_position left = *context.left_edge_top;
    surface_position right = *context.right_edge_top;
    int y = left.view_position.y();
    int max_y = y + context.halftriangle_height;
    max_y = std::min(max_y, target.pixel_height() - 1);

    int y_skip = context.compute_y_skip(y);
    
    if (y_skip != 0) {
        left.add_vwt(y_skip, *context.left_edge_delta);
        right.add_vwt(y_skip, *context.right_edge_delta);
        y += y_skip;
    }

    for (; y <= max_y; y += context.scanline_divisor) {
        int x = left.view_position.x();
        int max_x = right.view_position.x();
        surface_position pixel = left;
        surface_position delta;
        delta.compute_delta_vwt(left, right, max_x - x);
        max_x = std::min(max_x, target.pixel_width() - 1);

        if (x < 0) {
            pixel.add_vwt(-x, delta);
            x = 0;
        }

        for (; x <= max_x; ++x) {
            if (target.depth_at(x, y) < pixel.view_position.z())         
                target.set_pixel_overwriting_z_buffer(x, y, pixel.view_position.z(),
                                                      mat->shade(pixel.world_position,
                                                                 context.surface_normal,
                                                                 (context.eye - pixel.world_position).unit(),
                                                                 pixel.uv,
                                                                 *context.lights));

            pixel.add_vwt(delta); // TODO: skip view_position, use z alone
        }

        left.add_vwt(context.scanline_divisor, *context.left_edge_delta);
        right.add_vwt(context.scanline_divisor, *context.right_edge_delta);
    }
}

void triangle::visualize_normals(framebuffer &target, scene &parent_scene) const {
    const vector3f *view_data = parent_scene.view_coordinate_data();
    const vector3f *world_data = parent_scene.world_coordinate_data();
    const vector3f *normal_data = parent_scene.world_normal_data();
    const matrix4x4f &world_to_view = parent_scene.world_to_view();
    color3 yellow(1.0f, 1.0f, 0.0f);
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

        color3 light_color(source->specular());
        color3 reflected_color(light_color * mat->get_specular_reflectivity());

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
