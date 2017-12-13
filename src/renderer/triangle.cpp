#include "triangle.h"
#include "color.h"
#include "framebuffer.h"
#include "light.h"
#include "light_list.h"
#include "linear_transforms.h"
#include "material.h"
#include "math_util.h"
#include "matrix.h"
#include "mesh.h"
#include "scene.h"
#include <algorithm>

using namespace math;

triangle::triangle() {
}

triangle::triangle(int vi1, int vi2, int vi3,
                   const math::vector3f &uv1, const math::vector3f &uv2, const math::vector3f &uv3,
                   int ni1, int ni2, int ni3,
                   const material *mat) :
    vertex_index{vi1, vi2, vi3},
    vertex_uv{uv1, uv2, uv3},
    normal_index{ni1, ni2, ni3},
    mat(mat),
    has_distinct_normals(ni1 != ni2 || ni1 != ni3),
    has_uv_coordinates(true) {
}

triangle::triangle(int vi1, int vi2, int vi3, int ni1, int ni2, int ni3, const material *mat) :
    vertex_index{vi1, vi2, vi3},
    vertex_uv{vector3f(), vector3f(), vector3f()},
    normal_index{ni1, ni2, ni3},
    mat(mat),
    has_distinct_normals(ni1 != ni2 || ni1 != ni3),
    has_uv_coordinates(false) {
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
    return *this;
}

triangle::~triangle() {
}

void triangle::render(framebuffer &target, const mesh &parent_mesh, const scene &parent_scene) const {
    const vector4f *view_coord = parent_mesh.view_coordinate_data();

    for (int i = 0; i < 3; ++i)
        render_context.vtx(i).view_position = view_coord[vertex_index[i]].dehomo();

    if (triangle_winds_clockwise())
        return;

    const vector4f *world_coord = parent_mesh.world_coordinate_data();

    for (int i = 0; i < 3; ++i)
        render_context.vtx(i).world_position = world_coord[vertex_index[i]].dehomo();

    // plane clip
    if (parent_scene.visible_volume().clip(render_context.vtx(0).view_position,
                                           render_context.vtx(1).view_position,
                                           render_context.vtx(2).view_position))
        return;
    
    render_context.lights = &parent_scene.light_sources();
    render_context.tex = mat->get_texture_map();
    shading_model_t shading = parent_scene.get_shading_model();
    
    if (shading == flat)
        render_flat(target, parent_mesh, parent_scene);
    else if (shading == gouraud)
        render_gouraud(target, parent_mesh, parent_scene);
    else if (shading == phong)
        render_phong(target, parent_mesh, parent_scene);
}

void triangle::render_flat(framebuffer &target, const mesh &parent_mesh, const scene &parent_scene) const {
    const vector4f *world_normal = parent_mesh.world_normal_data();
    render_context.surface_midpoint = vector3f{0.0f, 0.0f, 0.0f};
    
    for (int i = 0; i < 3; ++i) {
        vertex_data &vtx = render_context.vtx(i);
        render_context.surface_midpoint += vtx.world_position;
        vtx.uv[0] = vertex_uv[i][0];
        vtx.uv[1] = vertex_uv[i][1];
    }

    render_context.surface_midpoint /= 3.0f;
    
    if (has_distinct_normals)
        render_context.surface_normal = world_normal[normal_index[0]].dehomo();
    else
        render_context.surface_normal = (world_normal[normal_index[0]].dehomo() +
                                         world_normal[normal_index[1]].dehomo() +
                                         world_normal[normal_index[2]].dehomo()) / 3.0f;

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

void triangle::render_gouraud(framebuffer &target, const mesh &parent_mesh, const scene &parent_scene) const {
    const vector4f *world_data = parent_mesh.world_coordinate_data();
    const vector4f *world_normal = parent_mesh.world_normal_data();
    
    for (int i = 0; i < 3; ++i) {
        vector3f vertex = world_data[vertex_index[i]].dehomo();
        render_context.vtx(i).shade = mat->shade_phong(vertex,
                                                       world_normal[normal_index[i]].dehomo(),
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

void triangle::render_phong(framebuffer &target, const mesh &parent_mesh, const scene &parent_scene) const {
    render_context.eye = parent_scene.get_eye_position();

    if (has_distinct_normals)
        render_smooth_phong(target, parent_mesh, parent_scene);
    else
        render_flat_phong(target, parent_mesh, parent_scene);
}

void triangle::render_smooth_phong(framebuffer &target, const mesh &parent_mesh, const scene &parent_scene) const {
    const vector4f *world_normal = parent_mesh.world_normal_data();
    
    for (int i = 0; i < 3; ++i)
        render_context.vtx(i).normal = world_normal[normal_index[i]].dehomo();

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

void triangle::render_flat_phong(framebuffer &target, const mesh &parent_mesh, const scene &parent_scene) const {
    const vector4f *world_normal = parent_mesh.world_normal_data();
    render_context.surface_normal = vector3f{0.0f, 0.0f, 0.0f};

    for (int i = 0; i < 3; ++i)
        render_context.surface_normal += world_normal[normal_index[i]].dehomo();

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
        y = 0;
    }

    color shade(mat->shade_flat(render_context.surface_midpoint, render_context.surface_normal, *render_context.lights));
    
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
            target.set_pixel(x, y, pixel.view_position.z(), shade);
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
            target.set_pixel(x, y, pixel.view_position.z(), pixel.shade);
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
            color shade(mat->shade_phong(pixel.view_position,
                                         pixel.normal.unit(),
                                         (render_context.eye - pixel.world_position).unit(),
                                         *render_context.lights));
            target.set_pixel(x, y, pixel.view_position.z(), shade);
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
            color shade(mat->shade_phong(pixel.view_position,
                                         render_context.surface_normal,
                                         (render_context.eye - pixel.world_position).unit(),
                                         *render_context.lights));
            target.set_pixel(x, y, pixel.view_position.z(), shade);
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
        y = 0;
    }

    color shade(mat->shade_flat(render_context.surface_midpoint, render_context.surface_normal, *render_context.lights));
    
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
            target.set_pixel(x, y, pixel.view_position.z(), shade * render_context.tex->at(pixel.uv[0], pixel.uv[1]));
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
            target.set_pixel(x, y, pixel.view_position.z(),
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
            shade *= mat->shade_phong(pixel.view_position,
                                      pixel.normal.unit(),
                                      (render_context.eye - pixel.world_position).unit(),
                                      *render_context.lights);
            target.set_pixel(x, y, pixel.view_position.z(), shade);
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
            shade *= mat->shade_phong(pixel.view_position,
                                      render_context.surface_normal,
                                      (render_context.eye - pixel.world_position).unit(),
                                      *render_context.lights);
            target.set_pixel(x, y, pixel.view_position.z(), shade);
            pixel.add_vwt(delta); // TODO: skip view_position, use z alone
        }

        left.add_vwt(*render_context.left_edge_delta);
        right.add_vwt(*render_context.right_edge_delta);
    }
}

void triangle::visualize_normals(framebuffer &target, const mesh &parent_mesh,
                                 scene &parent_scene, const matrix4x4f &world_to_view) const {
    const vector4f *view_data = parent_mesh.view_coordinate_data();
    const vector4f *world_data = parent_mesh.world_coordinate_data();
    const vector4f *normal_data = parent_mesh.world_normal_data();
    color yellow(1, 1, 0, 1);

    for (int i = 0; i < 3; ++i) {
        int vi = vertex_index[i], ni = normal_index[i];
        vector4f v(view_data[vi]); // view vertex
        vector4f wn(world_data[vi] + 0.3f * normal_data[ni]); // world normal, offset from vertex

        vector4f vn = world_to_view * wn;
        vn.divide_by_h();
        line::render(target, v.x(), v.y(), v.z(), yellow, vn.x(), vn.y(), vn.z(), yellow);
    }
}

void triangle::visualize_reflection_vectors(framebuffer &target, const mesh &parent_mesh,
                                            scene &parent_scene, const matrix4x4f &world_to_view) const {
    const vector4f *world_data = parent_mesh.world_coordinate_data();
    const vector4f *normal_data = parent_mesh.world_normal_data();

    vector3f wv[3] = {
        world_data[vertex_index[0]].dehomo(),
        world_data[vertex_index[1]].dehomo(),
        world_data[vertex_index[2]].dehomo()
    };

    vector3f wn[3] = {
        normal_data[normal_index[0]].dehomo(),
        normal_data[normal_index[1]].dehomo(),
        normal_data[normal_index[2]].dehomo()
    };

    // choose a point roughly in the middle of the triangle
    vector3f mid_point = ((wv[0] + wv[1]) / 2.0f + wv[2]) / 2.0f;
    vector3f mid_normal = ((wn[0] + wn[1]) / 2.0f + wn[2]) / 2.0f;
    mid_normal.normalize();

    // phong vectors
    for (const light *source: parent_scene.light_sources().get()) {
        vector3f light_vector = source->surface_to_light_unit(mid_point);
        float normal_dot_light(mid_normal.dot(light_vector));
        vector3f reflection_vector(2.0f * normal_dot_light * mid_normal - light_vector);

        vector4f p = world_to_view * mid_point.homo();
        vector4f l = world_to_view * (mid_point + 0.3f * light_vector).homo();
        vector4f r = world_to_view * (mid_point + 0.3f * reflection_vector).homo();
        vector4f n = world_to_view * (mid_point + 0.3f * mid_normal).homo();
        p.divide_by_h();
        l.divide_by_h();
        r.divide_by_h();
        n.divide_by_h();

        color lc(source->get_color());
        color rc(lc * mat->get_specular_reflectivity());
        color white(1.0f, 1.0f, 1.0f, 1.0f);

        line::render(target, p.x(), p.y(), p.z(), lc, l.x(), l.y(), l.z(), lc);
        line::render(target, p.x(), p.y(), p.z(), rc, l.x(), r.y(), r.z(), rc);
        line::render(target, p.x(), p.y(), p.z(), white, n.x(), n.y(), n.z(), white);
    }
}

void triangle::render_dumb(framebuffer &target, const mesh &parent_mesh, const scene &parent_scene) const {
    const vector4f *view_data = parent_mesh.view_coordinate_data();

    // vertex winding test
    vector4f v1 = view_data[vertex_index[0]];
    vector4f v2 = view_data[vertex_index[1]];
    vector4f v3 = view_data[vertex_index[2]];

    float sum = (v2.x() - v1.x()) * (v2.y() + v1.y()) +
                (v3.x() - v2.x()) * (v3.y() + v2.y()) +
                (v1.x() - v3.x()) * (v1.y() + v3.y());

    if (sum < 0.0f)
        return;

    // plane clip
    if (parent_scene.visible_volume().clip(v1, v2, v3))
        return;

    edge e[3] = { create_edge(0, 1, view_data), create_edge(1, 2, view_data), create_edge(0, 2, view_data) };
    int long_edge_index = find_long_edge(e, view_data);

    draw_half_triangle(e[long_edge_index], e[(long_edge_index + 1) % 3], target, parent_mesh, parent_scene);
    draw_half_triangle(e[long_edge_index], e[(long_edge_index + 2) % 3], target, parent_mesh, parent_scene);
}

triangle::edge triangle::create_edge(int vi1, int vi2, const vector4f *vertex_data) const {
    if (vertex_data[vertex_index[vi1]].y() < vertex_data[vertex_index[vi2]].y())
        return edge { vi1, vi2 };
    else
        return edge { vi2, vi1 };
}

int triangle::find_long_edge(edge *edges, const vector4f *vertex_data) const {
    float height[3] = {
        (vertex_data[vertex_index[edges[0].bottom]].y() - vertex_data[vertex_index[edges[0].top]].y()),
        (vertex_data[vertex_index[edges[1].bottom]].y() - vertex_data[vertex_index[edges[1].top]].y()),
        (vertex_data[vertex_index[edges[2].bottom]].y() - vertex_data[vertex_index[edges[2].top]].y())
    };

    int i = height[0] > height[1] ? 0 : 1;
    return height[i] > height[2] ? i : 2;
}

void triangle::draw_half_triangle(const edge &long_edge, const edge &short_edge, framebuffer &target,
                                  const mesh &parent_mesh, const scene &parent_scene) const {
    // TODO: maybe use vector& rather than copy? Maybe create that render_context thingy?
    // TODO: split function to avoid u/v computation when there is no texture, for
    // different reflection models etc.

    const vector4f *vertex_data = parent_mesh.view_coordinate_data();
    
    // long_edge is the one that needs two passes to draw, reaching from top y to bottom y.
    int long_top_y = vertex_data[vertex_index[long_edge.top]].y();
    int short_top_y = vertex_data[vertex_index[short_edge.top]].y();
    int long_bottom_y = vertex_data[vertex_index[long_edge.bottom]].y();
    int short_bottom_y = vertex_data[vertex_index[short_edge.bottom]].y();

    if (long_bottom_y == long_top_y || short_bottom_y == short_top_y)
        return;

    float height_1 = long_bottom_y - long_top_y;
    float height_2 = short_bottom_y - short_top_y;

    int long_top_x = vertex_data[vertex_index[long_edge.top]].x();
    float long_top_z = vertex_data[vertex_index[long_edge.top]].z();
    int long_bottom_x = vertex_data[vertex_index[long_edge.bottom]].x();
    float long_bottom_z = vertex_data[vertex_index[long_edge.bottom]].z();
    int short_top_x = vertex_data[vertex_index[short_edge.top]].x();
    float short_top_z = vertex_data[vertex_index[short_edge.top]].z();
    int short_bottom_x = vertex_data[vertex_index[short_edge.bottom]].x();
    float short_bottom_z = vertex_data[vertex_index[short_edge.bottom]].z();

    const vector4f *normal_data = parent_mesh.world_normal_data();
    const vector4f *world_data = parent_mesh.world_coordinate_data();
    
    int y_offset = short_top_y - long_top_y; // 0 or rows already drawn
    float x1_delta = (long_bottom_x - long_top_x) / height_1; // screen x coordinate
    float x2_delta = (short_bottom_x - short_top_x) / height_2;
    float z1_delta = (long_bottom_z - long_top_z) / height_1; // depth buffer z cooedinate
    float z2_delta = (short_bottom_z - short_top_z) / height_2;
    float u1_delta = (vertex_uv[long_edge.bottom].x() - vertex_uv[long_edge.top].x()) / height_1; // texture u
    float u2_delta = (vertex_uv[short_edge.bottom].x() - vertex_uv[short_edge.top].x()) / height_2;
    float v1_delta = (vertex_uv[long_edge.bottom].y() - vertex_uv[long_edge.top].y()) / height_1; // texture v
    float v2_delta = (vertex_uv[short_edge.bottom].y() - vertex_uv[short_edge.top].y()) / height_2;
    vector3f n1_delta = (normal_data[normal_index[long_edge.bottom]] - // pixel normal in worĺd coordinate system
                         normal_data[normal_index[long_edge.top]]).dehomo() / height_1;
    vector3f n2_delta = (normal_data[normal_index[short_edge.bottom]] -
                         normal_data[normal_index[short_edge.top]]).dehomo() / height_2;
    vector3f w1_delta = (world_data[vertex_index[long_edge.bottom]] - // pixel in worĺd coordinate system
                         world_data[vertex_index[long_edge.top]]).dehomo() / height_1;
    vector3f w2_delta = (world_data[vertex_index[short_edge.bottom]] -
                         world_data[vertex_index[short_edge.top]]).dehomo() / height_2;

    float x1 = long_top_x + y_offset * x1_delta;
    float x2 = short_top_x;
    float z1 = long_top_z + y_offset * z1_delta;
    float z2 = short_top_z;
    float u1 = vertex_uv[long_edge.top].x() + y_offset * u1_delta;
    float u2 = vertex_uv[short_edge.top].x();
    float v1 = vertex_uv[long_edge.top].y() + y_offset * v1_delta;
    float v2 = vertex_uv[short_edge.top].y();
    vector3f n1 = normal_data[normal_index[long_edge.top]].dehomo() + y_offset * n1_delta;
    vector3f n2 = normal_data[normal_index[short_edge.top]].dehomo();
    vector3f w1 = world_data[vertex_index[long_edge.top]].dehomo() + y_offset * w1_delta;
    vector3f w2 = world_data[vertex_index[short_edge.top]].dehomo();

    float min_y = short_top_y; // screen y coordinate
    float max_y = short_bottom_y;

    if (min_y < 0) {
        x1 += -min_y * x1_delta;
        x2 += -min_y * x2_delta;
        z1 += -min_y * z1_delta;
        z2 += -min_y * z2_delta;
        u1 += -min_y * u1_delta;
        u2 += -min_y * u2_delta;
        v1 += -min_y * v1_delta;
        v2 += -min_y * v2_delta;
        n1 += -min_y * n1_delta;
        n2 += -min_y * n2_delta;
        w1 += -min_y * w1_delta;
        w2 += -min_y * w2_delta;
        min_y = 0;
    }

    max_y = std::min((int)max_y, target.pixel_height() - 1);

    // TODO: check z-coords as well

    const texture *tex = mat->get_texture_map(); // TODO: refactor
    vector3f eye = parent_scene.get_eye_position();
    const light_list &light_sources = parent_scene.light_sources();
    
    int min_x, max_x;
    float z, z_delta;
    float u, u_delta;
    float v, v_delta;
    vector3f n, n_delta;
    vector3f w, w_delta;
    
    for (int y = min_y; y <= max_y; ++y) {
        // TODO: precompute min and max, don't redo once per line
        if (x1 <= x2) {
            min_x = x1;
            max_x = x2;
            z = z1;
            z_delta = (z2 - z1) / (x2 - x1 + 1);
            u = u1;
            u_delta = (u2 - u1) / (x2 - x1 + 1);
            v = v1;
            v_delta = (v2 - v1) / (x2 - x1 + 1);
            n = n1;
            n_delta = (n2 - n1) / (x2 - x1 + 1);
            w = w1;
            w_delta = (w2 - w1) / (x2 - x1 + 1);
        }
        else {
            min_x = x2;
            max_x = x1;
            z = z2;
            z_delta = (z1 - z2) / (x1 - x2 + 1);
            u = u2;
            u_delta = (u1 - u2) / (x1 - x2 + 1);
            v = v2;
            v_delta = (v1 - v2) / (x1 - x2 + 1);
            n = n2;
            n_delta = (n1 - n2) / (x1 - x2 + 1);
            w = w2;
            w_delta = (w1 - w2) / (x1 - x2 + 1);
        }

        if (min_x < 0) {
            z += -min_x * z_delta;
            u += -min_x * u_delta;
            v += -min_x * v_delta;
            n += -min_x * n_delta;
            w += -min_x * w_delta;
            min_x = 0;
        }

        max_x = std::min(max_x, target.pixel_width() - 1);

        // TODO: n.unit() might be possible to optimize away

        if (tex)
            for (int x = min_x; x <= max_x; ++x, z += z_delta, u += u_delta, v += v_delta, n += n_delta, w += w_delta) {
                color surface_color = tex->at(u, v);
                color shade = mat->shade_phong(w, n.unit(), (eye - w).unit(), light_sources);
                target.set_pixel_unchecked(x, y, z, shade * surface_color);
            }
        else {
            color white(1.0f, 1.0f, 1.0f, 1.0f);

            for (int x = min_x; x <= max_x; ++x, z += z_delta, n += n_delta, w += w_delta) {
                color shade = mat->shade_phong(w, n.unit(), (eye - w).unit(), light_sources);
                target.set_pixel_unchecked(x, y, z, shade);
            }
        }

        x1 += x1_delta;
        x2 += x2_delta;
        z1 += z1_delta;
        z2 += z2_delta;
        u1 += u1_delta;
        u2 += u2_delta;
        v1 += v1_delta;
        v2 += v2_delta;
        n1 += n1_delta;
        n2 += n2_delta;
        w1 += w1_delta;
        w2 += w2_delta;
    }
}

triangle_render triangle::render_context;

