#include "material_library.h"

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
    
    materials[name] = material(get_texture(texture_name));
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
