#include "material_library.h"

using namespace math;
using namespace std;

material_library::material_library() {
    null_material.set_ambient_reflectivity(color(1.0f, 1.0f, 1.0f, 0.0f));
    null_material.set_diffuse_reflectivity(color(1.0f, 1.0f, 1.0f, 0.0f));
    null_material.set_specular_reflectivity(color(1.0f, 1.0f, 1.0f, 0.0f));
}

material_library::~material_library() {
}

bool material_library::add_texture(const std::string &name, const std::string &filename) {
    if (textures.find(name) != textures.end())
        return false;

    textures[name] = texture::load_png(filename);
    return true;
}

bool material_library::add_material(const std::string &name, unique_ptr<material> mat) {
    if (materials.find(name) != materials.end())
        return false;
    else {
        materials[name] = move(mat);
        return true;
    }
}

const material *material_library::get_material(const std::string &name) const {
    const auto it = materials.find(name);

    if (it == materials.end())
        throw material_library_exception();

    return it->second.get();
}

const material *material_library::get_null_material() const {
    return &null_material;
}

const texture *material_library::get_texture(const std::string &name) const {
    const auto it = textures.find(name);

    if (it == textures.end())
        throw material_library_exception();

    return &it->second;
}
