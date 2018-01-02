#include "surface_position.h"
#include "util.h"
#include <sstream>

void surface_position::add_vwnts(const surface_position &delta) {
    view_position += delta.view_position;
    world_position += delta.world_position;
    normal += delta.normal;
    uv += delta.uv;
    shade += delta.shade;
}

void surface_position::add_vwnt(const surface_position &delta) {
    view_position += delta.view_position;
    world_position += delta.world_position;
    normal += delta.normal;
    uv += delta.uv;
}

void surface_position::add_vwt(const surface_position &delta) {
    view_position += delta.view_position;
    world_position += delta.world_position;
    uv += delta.uv;
}

void surface_position::add_vwn(const surface_position &delta) {
    view_position += delta.view_position;
    world_position += delta.world_position;
    normal += delta.normal;
}

void surface_position::add_vts(const surface_position &delta) {
    view_position += delta.view_position;
    uv += delta.uv;
    shade += delta.shade;
}

void surface_position::add_vw(const surface_position &delta) {
    view_position += delta.view_position;
    world_position += delta.world_position;
}

void surface_position::add_vt(const surface_position &delta) {
    view_position += delta.view_position;
    uv += delta.uv;
}

void surface_position::add_vs(const surface_position &delta) {
    view_position += delta.view_position;
    shade += delta.shade;
}

void surface_position::add_v(const surface_position &delta) {
    view_position += delta.view_position;
}

void surface_position::add_vwnts(float multiplier, const surface_position &delta) {
    view_position += multiplier * delta.view_position;
    world_position += multiplier * delta.world_position;
    normal += multiplier * delta.normal;
    uv += multiplier * delta.uv;
    shade += multiplier * delta.shade;
}

void surface_position::add_vwnt(float multiplier, const surface_position &delta) {
    view_position += multiplier * delta.view_position;
    world_position += multiplier * delta.world_position;
    normal += multiplier * delta.normal;
    uv += multiplier * delta.uv;
}

void surface_position::add_vts(float multiplier, const surface_position &delta) {
    view_position += multiplier * delta.view_position;
    uv += multiplier * delta.uv;
    shade += multiplier * delta.shade;
}

void surface_position::add_vwt(float multiplier, const surface_position &delta) {
    view_position += multiplier * delta.view_position;
    world_position += multiplier * delta.world_position;
    uv += multiplier * delta.uv;
}

void surface_position::add_vwn(float multiplier, const surface_position &delta) {
    view_position += multiplier * delta.view_position;
    world_position += multiplier * delta.world_position;
    normal += multiplier * delta.normal;
}

void surface_position::add_vw(float multiplier, const surface_position &delta) {
    view_position += multiplier * delta.view_position;
    world_position += multiplier * delta.world_position;
}

void surface_position::add_vt(float multiplier, const surface_position &delta) {
    view_position += multiplier * delta.view_position;
    uv += multiplier * delta.uv;
}

void surface_position::add_vs(float multiplier, const surface_position &delta) {
    view_position += multiplier * delta.view_position;
    shade += multiplier * delta.shade;
}

void surface_position::add_v(float multiplier, const surface_position &delta) {
    view_position += multiplier * delta.view_position;
}

void surface_position::compute_delta_vwnts(const surface_position &v1, const surface_position &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    world_position = (v2.world_position - v1.world_position) / steps;
    normal = (v2.normal - v1.normal) / steps;
    uv = (v2.uv - v1.uv) / steps;
    shade = (v2.shade - v1.shade) / steps;
}

void surface_position::compute_delta_vwnt(const surface_position &v1, const surface_position &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    world_position = (v2.world_position - v1.world_position) / steps;
    normal = (v2.normal - v1.normal) / steps;
    uv = (v2.uv - v1.uv) / steps;
}

void surface_position::compute_delta_vwt(const surface_position &v1, const surface_position &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    world_position = (v2.world_position - v1.world_position) / steps;
    uv = (v2.uv - v1.uv) / steps;
}

void surface_position::compute_delta_vwn(const surface_position &v1, const surface_position &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    world_position = (v2.world_position - v1.world_position) / steps;
    normal = (v2.normal - v1.normal) / steps;
}

void surface_position::compute_delta_vts(const surface_position &v1, const surface_position &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    uv = (v2.uv - v1.uv) / steps;
    shade = (v2.shade - v1.shade) / steps;
}

void surface_position::compute_delta_vw(const surface_position &v1, const surface_position &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    world_position = (v2.world_position - v1.world_position) / steps;
}

void surface_position::compute_delta_vt(const surface_position &v1, const surface_position &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    uv = (v2.uv - v1.uv) / steps;
}

void surface_position::compute_delta_vs(const surface_position &v1, const surface_position &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
    shade = (v2.shade - v1.shade) / steps;
}

void surface_position::compute_delta_v(const surface_position &v1, const surface_position &v2, float steps) {
    view_position = (v2.view_position - v1.view_position) / steps;
}

std::string surface_position::to_string() const {
    std::ostringstream o;
    o << "v = " << util::to_string(view_position)
      << " w = " << util::to_string(world_position)
      << " n = " << util::to_string(normal)
      << " uv = " << util::to_string(uv)
      << " s = " << shade.to_string();
    return o.str();
}
