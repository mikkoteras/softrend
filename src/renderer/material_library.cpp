#include "material_library.h"

using namespace math;

material_library::material_library() {
}

material_library::~material_library() {
}

bool material_library::add_texture(const std::string &name, const std::string &filename) {
    if (textures.find(name) != textures.end())
        return false;

    textures[name] = texture::load_png(filename);
    return true;
}

bool material_library::add_material(const std::string &name, const std::string &texture_name) {
    if (materials.find(name) != materials.end())
        return false;

    material::model model;
    model.ambient_reflectivity = vector3f{0.f, 0.f, 0.f};
    model.reflectivity = vector3f{0.f, 0.f, 0.f};
    model.specular_reflectivity = vector3f{0.f, 0.f, 0.f};
    model.material_dissolve = 0.0f;
    model.specular_exponent = 0.0f;
    model.illumination_model = 0;
    materials.emplace(name, material(model, get_texture(texture_name)));
    return true;
}

const material *material_library::get_material(const std::string &name) const {
    const auto it = materials.find(name);

    if (it == materials.end())
        throw material_library_exception();

    return &it->second;
}

const texture *material_library::get_texture(const std::string &name) const {
    const auto it = textures.find(name);

    if (it == textures.end())
        throw material_library_exception();

    return &it->second;
}
