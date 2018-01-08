#ifndef MATERIAL_LIBRARY_H
#define MATERIAL_LIBRARY_H

#include "material.h"
#include "specular_material.h"
#include "texture.h"
#include <map>
#include <memory>
#include <string>

class material_library_exception {};

class material_library {
public:
    material_library();
    ~material_library();

    bool add_texture(const std::string &name, const std::string &filename);
    bool add_material(const std::string &name, std::unique_ptr<material> mat);
    const material *get_material(const std::string &name) const;
    const material *get_null_material() const;
    const texture *get_texture(const std::string &name) const;

private:
    std::map<std::string, texture> textures;
    std::map<std::string, std::unique_ptr<material>> materials;
    specular_material null_material;
};

#endif
