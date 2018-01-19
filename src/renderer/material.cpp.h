template<combined_interpolation_mode_t mode>
color4 material::shade(const surface_position &point, const scene_render_context &scene) const {
    return shade_phong(point, scene);
}
